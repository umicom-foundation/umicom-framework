/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/timer_trace.c
 *
 * PURPOSE:
 *   Verify a teaching clock/event trace with checked deadlines and copied
 *   state. A tick is a logical unit, never an assumed wall-clock duration.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/timer_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum EducationWaitState {
    EDUCATION_WAIT_PENDING = 1,
    EDUCATION_WAIT_SIGNALLED = 2,
    EDUCATION_WAIT_TIMED_OUT = 3,
    EDUCATION_WAIT_CANCELLED = 4
} EducationWaitState;

typedef struct EducationWait {
    uint32_t identity;
    uint32_t key;
    uint64_t deadline;
    EducationWaitState state;
} EducationWait;

typedef struct EducationTimerModel {
    EducationWait waits[UMI_EDUCATION_TIMER_MAX_WAITS];
    uint32_t used;
    uint32_t pending;
    uint64_t now;
} EducationTimerModel;

static uint16_t Read16(const unsigned char *bytes)
{
    return (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8U));
}
static uint32_t Read32(const unsigned char *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8U) |
           ((uint32_t)bytes[2] << 16U) | ((uint32_t)bytes[3] << 24U);
}
static uint64_t Read64(const unsigned char *bytes)
{
    return (uint64_t)Read32(bytes) | ((uint64_t)Read32(bytes + 4U) << 32U);
}
static EducationWait *FindWait(EducationTimerModel *model, uint32_t identity)
{
    for (uint32_t i = 0U; i < model->used; ++i)
        if (model->waits[i].identity == identity) return &model->waits[i];
    return NULL;
}

/* Only ADVANCE updates the logical clock. At the threshold, timeout wins over
 * a subsequent signal; a callback becoming runnable later is a separate act. */
static UmiStatus Advance(EducationTimerModel *model,
                         UmiEducationTimerReport *report, uint64_t tick)
{
    if (tick <= model->now) return UMI_STATUS_PARSE_ERROR;
    model->now = tick;
    ++report->advances;
    for (uint32_t i = 0U; i < model->used; ++i) {
        EducationWait *wait = &model->waits[i];
        if (wait->state == EDUCATION_WAIT_PENDING && wait->deadline != 0U &&
            wait->deadline <= tick) {
            wait->state = EDUCATION_WAIT_TIMED_OUT;
            --model->pending;
            ++report->timedOut;
        }
    }
    return UMI_STATUS_OK;
}

static UmiStatus Apply(EducationTimerModel *model, UmiEducationTimerReport *report,
                       unsigned event, uint32_t identity, uint32_t key,
                       uint64_t tick, uint64_t value)
{
    if (event == UMI_EDUCATION_TIMER_ADVANCE) {
        if (identity != 0U || key != 0U || value != 0U)
            return UMI_STATUS_PARSE_ERROR;
        return Advance(model, report, tick);
    }
    /* An action cannot secretly advance time: the trace must show the clock
     * transition explicitly, allowing a reader to reproduce its event order. */
    if (identity == 0U || key == 0U || tick != model->now)
        return UMI_STATUS_PARSE_ERROR;
    EducationWait *wait = FindWait(model, identity);
    if (event == UMI_EDUCATION_TIMER_WAIT) {
        if (wait != NULL) return UMI_STATUS_PARSE_ERROR;
        if (model->used >= UMI_EDUCATION_TIMER_MAX_WAITS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        if (value > UMI_EDUCATION_TIMER_MAX_DELAY)
            return UMI_STATUS_PARSE_ERROR;
        if (value != 0U && model->now > UINT64_MAX - value)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        wait = &model->waits[model->used++];
        wait->identity = identity;
        wait->key = key;
        wait->deadline = value == 0U ? 0U : model->now + value;
        wait->state = EDUCATION_WAIT_PENDING;
        ++model->pending;
        ++report->waits;
    } else {
        /* Closing, signalling or cancelling a wait is a one-time decision.
         * Neither late events nor notifications to an old identity can touch
         * a later registration. Keys match an event, not a permission secret. */
        if (wait == NULL || wait->state != EDUCATION_WAIT_PENDING ||
            wait->key != key || value != 0U ||
            (wait->deadline != 0U && tick >= wait->deadline))
            return UMI_STATUS_PARSE_ERROR;
        switch (event) {
        case UMI_EDUCATION_TIMER_SIGNAL:
            wait->state = EDUCATION_WAIT_SIGNALLED;
            ++report->signalled;
            break;
        case UMI_EDUCATION_TIMER_CANCEL:
            wait->state = EDUCATION_WAIT_CANCELLED;
            ++report->cancelled;
            break;
        default:
            return UMI_STATUS_PARSE_ERROR;
        }
        --model->pending;
    }
    if (model->pending > report->peakPending) report->peakPending = model->pending;
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationTimerTraceInspect(const void *bytes, size_t length,
                                         UmiEducationTimerReport *report)
{
    static const unsigned char MAGIC[8] = {'U','M','I','T','M','R','1',0};
    if (bytes == NULL || report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_EDUCATION_TIMER_MAX_FILE_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length < UMI_EDUCATION_TIMER_HEADER_BYTES) return UMI_STATUS_PARSE_ERROR;
    const unsigned char *input = bytes;
    if (memcmp(input, MAGIC, sizeof(MAGIC)) != 0 ||
        Read16(input + 8U) != 1U || Read16(input + 10U) != 0U)
        return UMI_STATUS_PARSE_ERROR;
    const uint32_t count = Read32(input + 12U);
    if (count > UMI_EDUCATION_TIMER_MAX_RECORDS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length != UMI_EDUCATION_TIMER_HEADER_BYTES +
                  (size_t)count * UMI_EDUCATION_TIMER_RECORD_BYTES)
        return UMI_STATUS_PARSE_ERROR;

    EducationTimerModel model = {0};
    UmiEducationTimerReport next = {0};
    next.recordCount = count;
    for (uint32_t i = 0U; i < count; ++i) {
        const unsigned char *record = input + UMI_EDUCATION_TIMER_HEADER_BYTES +
            (size_t)i * UMI_EDUCATION_TIMER_RECORD_BYTES;
        if (Read32(record) != i + 1U || record[5] != 0U ||
            Read16(record + 6U) != 0U)
            return UMI_STATUS_PARSE_ERROR;
        const UmiStatus status = Apply(&model, &next, (unsigned)record[4],
            Read32(record + 8U), Read32(record + 12U),
            Read64(record + 16U), Read64(record + 24U));
        if (status != UMI_STATUS_OK) return status;
    }
    next.pendingAtEnd = model.pending;
    next.distinctIdentities = model.used;
    next.lastTick = model.now;
    uint64_t smallest = 0U;
    for (uint32_t i = 0U; i < model.used; ++i) {
        const EducationWait *wait = &model.waits[i];
        if (wait->state == EDUCATION_WAIT_PENDING && wait->deadline != 0U &&
            (smallest == 0U || wait->deadline < smallest))
            smallest = wait->deadline;
    }
    next.nextFiniteDeadline = smallest;
    *report = next;
    return UMI_STATUS_OK;
}

/* The earlier wrapper published before closing its input. Retain it for
 * review; the replacement below stages the report until all file I/O succeeds,
 * matching the shared inspection contract used by IDE and education clients. */
#if 0
UmiStatus UmiEducationTimerTraceInspectFile(const char *path,
                                             UmiEducationTimerReport *report)
{
    if (path == NULL || *path == '\0' || report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *stream = fopen(path, "rb");
    if (stream == NULL) return UMI_STATUS_NOT_FOUND;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    if (fseek(stream, 0L, SEEK_END) == 0) {
        const long end = ftell(stream);
        if (end >= 0L && (unsigned long)end > UMI_EDUCATION_TIMER_MAX_FILE_BYTES)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (end >= 0L && fseek(stream, 0L, SEEK_SET) == 0) {
            const size_t length = (size_t)end;
            unsigned char *buffer = malloc(length != 0U ? length : 1U);
            if (buffer == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                if (fread(buffer, 1U, length, stream) == length)
                    status = UmiEducationTimerTraceInspect(buffer, length, report);
                free(buffer);
            }
        }
    }
    if (fclose(stream) != 0 && status == UMI_STATUS_OK)
        return UMI_STATUS_IO_ERROR;
    return status;
}
#endif
UmiStatus UmiEducationTimerTraceInspectFile(const char *path,
                                             UmiEducationTimerReport *report)
{
    if (path == NULL || *path == '\0' || report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *stream = fopen(path, "rb");
    if (stream == NULL) return UMI_STATUS_NOT_FOUND;
    UmiEducationTimerReport candidate = {0};
    UmiStatus status = UMI_STATUS_IO_ERROR;
    if (fseek(stream, 0L, SEEK_END) == 0) {
        const long end = ftell(stream);
        if (end >= 0L && (unsigned long)end > UMI_EDUCATION_TIMER_MAX_FILE_BYTES)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (end >= 0L && fseek(stream, 0L, SEEK_SET) == 0) {
            const size_t length = (size_t)end;
            unsigned char *buffer = malloc(length != 0U ? length : 1U);
            if (buffer == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                if (fread(buffer, 1U, length, stream) == length)
                    status = UmiEducationTimerTraceInspect(buffer, length, &candidate);
                free(buffer);
            }
        }
    }
    if (fclose(stream) != 0 && status == UMI_STATUS_OK)
        return UMI_STATUS_IO_ERROR;
    /* Closing the input is part of the file operation. Only a complete read
     * and successful close may replace the caller's previous report. */
    if (status == UMI_STATUS_OK) *report = candidate;
    return status;
}
