/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/schedule_trace.c
 *
 * PURPOSE:
 *   Validate task state, FIFO fairness and matching wait/wake events from
 *   owned classroom trace bytes without modifying any live scheduler.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/schedule_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum EducationTaskState {
    EDUCATION_TASK_READY = 1,
    EDUCATION_TASK_RUNNING = 2,
    EDUCATION_TASK_WAITING = 3,
    EDUCATION_TASK_STOPPED = 4
} EducationTaskState;

typedef struct EducationTask {
    uint32_t identity;
    uint32_t waitKey;
    EducationTaskState state;
} EducationTask;

typedef struct EducationModel {
    EducationTask tasks[UMI_EDUCATION_SCHEDULE_MAX_TASKS];
    uint32_t used;
    uint32_t ready[UMI_EDUCATION_SCHEDULE_MAX_TASKS];
    uint32_t head;
    uint32_t readyCount;
    uint32_t waitingCount;
    uint32_t runningIdentity;
    uint32_t liveCount;
} EducationModel;

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
static EducationTask *FindTask(EducationModel *model, uint32_t identity)
{
    for (uint32_t i = 0U; i < model->used; ++i)
        if (model->tasks[i].identity == identity) return &model->tasks[i];
    return NULL;
}
static int Enqueue(EducationModel *model, uint32_t identity)
{
    if (model->readyCount == UMI_EDUCATION_SCHEDULE_MAX_TASKS) return 0;
    const uint32_t tail = (model->head + model->readyCount) % UMI_EDUCATION_SCHEDULE_MAX_TASKS;
    model->ready[tail] = identity;
    ++model->readyCount;
    return 1;
}

/* Prepare the complete next transition before publishing any state. The model
 * is private to this parse, but the separate transition cases are intentionally
 * visible for review and to keep their contracts comprehensible. */
static UmiStatus Apply(EducationModel *model, UmiEducationScheduleReport *report,
                       unsigned event, uint32_t identity, uint32_t detail)
{
    if (identity == 0U) return UMI_STATUS_PARSE_ERROR;
    EducationTask *task = FindTask(model, identity);
    switch (event) {
    case UMI_EDUCATION_SCHEDULE_ADMIT:
        if (task != NULL || detail != 0U) return UMI_STATUS_PARSE_ERROR;
        if (model->used == UMI_EDUCATION_SCHEDULE_MAX_TASKS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        task = &model->tasks[model->used];
        task->identity = identity;
        task->state = EDUCATION_TASK_READY;
        task->waitKey = 0U;
        if (!Enqueue(model, identity)) return UMI_STATUS_CAPACITY_EXCEEDED;
        ++model->used;
        ++model->liveCount;
        ++report->admitted;
        break;
    case UMI_EDUCATION_SCHEDULE_DISPATCH:
        if (detail != 0U || model->runningIdentity != 0U ||
            model->readyCount == 0U || model->ready[model->head] != identity ||
            task == NULL || task->state != EDUCATION_TASK_READY)
            return UMI_STATUS_PARSE_ERROR;
        model->head = (model->head + 1U) % UMI_EDUCATION_SCHEDULE_MAX_TASKS;
        --model->readyCount;
        task->state = EDUCATION_TASK_RUNNING;
        model->runningIdentity = identity;
        ++report->dispatched;
        break;
    case UMI_EDUCATION_SCHEDULE_YIELD:
        if (detail != 0U || task == NULL || task->state != EDUCATION_TASK_RUNNING ||
            model->runningIdentity != identity) return UMI_STATUS_PARSE_ERROR;
        if (!Enqueue(model, identity)) return UMI_STATUS_CAPACITY_EXCEEDED;
        task->state = EDUCATION_TASK_READY;
        model->runningIdentity = 0U;
        ++report->yielded;
        break;
    case UMI_EDUCATION_SCHEDULE_WAIT:
        if (detail == 0U || task == NULL || task->state != EDUCATION_TASK_RUNNING ||
            model->runningIdentity != identity) return UMI_STATUS_PARSE_ERROR;
        task->state = EDUCATION_TASK_WAITING;
        task->waitKey = detail;
        model->runningIdentity = 0U;
        ++model->waitingCount;
        ++report->waited;
        break;
    case UMI_EDUCATION_SCHEDULE_WAKE:
        if (detail == 0U || task == NULL || task->state != EDUCATION_TASK_WAITING ||
            task->waitKey != detail) return UMI_STATUS_PARSE_ERROR;
        if (!Enqueue(model, identity)) return UMI_STATUS_CAPACITY_EXCEEDED;
        task->state = EDUCATION_TASK_READY;
        task->waitKey = 0U;
        --model->waitingCount;
        ++report->awakened;
        break;
    case UMI_EDUCATION_SCHEDULE_EXIT:
        if (detail != 0U || task == NULL || task->state != EDUCATION_TASK_RUNNING ||
            model->runningIdentity != identity) return UMI_STATUS_PARSE_ERROR;
        task->state = EDUCATION_TASK_STOPPED;
        model->runningIdentity = 0U;
        --model->liveCount;
        ++report->exited;
        break;
    case UMI_EDUCATION_SCHEDULE_CANCEL:
        if (detail == 0U || task == NULL || task->state != EDUCATION_TASK_WAITING ||
            task->waitKey != detail) return UMI_STATUS_PARSE_ERROR;
        task->state = EDUCATION_TASK_STOPPED;
        task->waitKey = 0U;
        --model->waitingCount;
        --model->liveCount;
        ++report->cancelled;
        break;
    default:
        return UMI_STATUS_PARSE_ERROR;
    }
    if (model->readyCount > report->readyPeak) report->readyPeak = model->readyCount;
    if (model->waitingCount > report->waitingPeak) report->waitingPeak = model->waitingCount;
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationScheduleTraceInspect(const void *bytes, size_t length,
                                            UmiEducationScheduleReport *report)
{
    static const unsigned char MAGIC[8] = {'U','M','I','S','C','H','1',0};
    if (bytes == NULL || report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_EDUCATION_SCHEDULE_MAX_FILE_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length < UMI_EDUCATION_SCHEDULE_HEADER_BYTES) return UMI_STATUS_PARSE_ERROR;
    const unsigned char *input = bytes;
    if (memcmp(input, MAGIC, sizeof(MAGIC)) != 0 ||
        Read16(input + 8U) != 1U || Read16(input + 10U) != 0U)
        return UMI_STATUS_PARSE_ERROR;
    const uint32_t count = Read32(input + 12U);
    if (count > UMI_EDUCATION_SCHEDULE_MAX_RECORDS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length != UMI_EDUCATION_SCHEDULE_HEADER_BYTES +
                     (size_t)count * UMI_EDUCATION_SCHEDULE_RECORD_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    EducationModel model = {0};
    UmiEducationScheduleReport next = {0};
    next.recordCount = count;
    uint64_t lastTick = 0U;
    for (uint32_t index = 0U; index < count; ++index) {
        const unsigned char *record = input + UMI_EDUCATION_SCHEDULE_HEADER_BYTES +
            (size_t)index * UMI_EDUCATION_SCHEDULE_RECORD_BYTES;
        const uint64_t tick = Read64(record + 16U);
        if (Read32(record) != index + 1U || record[5] != 0U ||
            Read16(record + 6U) != 0U || (index != 0U && tick < lastTick))
            return UMI_STATUS_PARSE_ERROR;
        const UmiStatus status = Apply(&model, &next, (unsigned)record[4],
                                       Read32(record + 8U), Read32(record + 12U));
        if (status != UMI_STATUS_OK) return status;
        lastTick = tick;
    }
    next.readyAtEnd = model.readyCount;
    next.waitingAtEnd = model.waitingCount;
    next.liveAtEnd = model.liveCount;
    next.runningAtEnd = model.runningIdentity;
    next.lastTick = lastTick;
    *report = next;
    return UMI_STATUS_OK;
}

/* The earlier wrapper published before closing its input. Retain it for
 * review; the replacement below stages the report until all file I/O succeeds,
 * matching the shared inspection contract used by IDE and education clients. */
#if 0
UmiStatus UmiEducationScheduleTraceInspectFile(const char *path,
                                                UmiEducationScheduleReport *report)
{
    if (path == NULL || path[0] == '\0' || report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *stream = fopen(path, "rb");
    if (stream == NULL) return UMI_STATUS_NOT_FOUND;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    if (fseek(stream, 0L, SEEK_END) == 0) {
        const long end = ftell(stream);
        if (end >= 0L && (unsigned long)end > UMI_EDUCATION_SCHEDULE_MAX_FILE_BYTES)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (end >= 0L && fseek(stream, 0L, SEEK_SET) == 0) {
            const size_t length = (size_t)end;
            unsigned char *buffer = malloc(length != 0U ? length : 1U);
            if (buffer == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                if (fread(buffer, 1U, length, stream) == length)
                    status = UmiEducationScheduleTraceInspect(buffer, length, report);
                free(buffer);
            }
        }
    }
    if (fclose(stream) != 0 && status == UMI_STATUS_OK)
        return UMI_STATUS_IO_ERROR;
    return status;
}
#endif
UmiStatus UmiEducationScheduleTraceInspectFile(const char *path,
                                                UmiEducationScheduleReport *report)
{
    if (path == NULL || path[0] == '\0' || report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *stream = fopen(path, "rb");
    if (stream == NULL) return UMI_STATUS_NOT_FOUND;
    UmiEducationScheduleReport candidate = {0};
    UmiStatus status = UMI_STATUS_IO_ERROR;
    if (fseek(stream, 0L, SEEK_END) == 0) {
        const long end = ftell(stream);
        if (end >= 0L && (unsigned long)end > UMI_EDUCATION_SCHEDULE_MAX_FILE_BYTES)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (end >= 0L && fseek(stream, 0L, SEEK_SET) == 0) {
            const size_t length = (size_t)end;
            unsigned char *buffer = malloc(length != 0U ? length : 1U);
            if (buffer == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                if (fread(buffer, 1U, length, stream) == length)
                    status = UmiEducationScheduleTraceInspect(buffer, length, &candidate);
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
