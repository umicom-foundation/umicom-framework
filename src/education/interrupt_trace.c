/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/interrupt_trace.c
 * PURPOSE: Validate an explicitly versioned, bounded, read-only classroom
 *          interrupt ownership trace; preserve caller reports on refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/interrupt_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum InterruptStage {
    INTERRUPT_FREE = 0,
    INTERRUPT_LEASED,
    INTERRUPT_ARMED,
    INTERRUPT_SERVICING,
    INTERRUPT_ACKNOWLEDGED,
    INTERRUPT_FAULTED
} InterruptStage;

typedef struct InterruptSource {
    InterruptStage stage;
    uint32_t owner;
    uint32_t generation;
    uint32_t lastGeneration;
} InterruptSource;

typedef struct InterruptModel {
    InterruptSource sources[UMI_EDUCATION_INTERRUPT_MAX_SOURCES];
    uint64_t tick;
    uint32_t owned;
} InterruptModel;

static uint16_t Read16(const unsigned char *input)
{
    return (uint16_t)((uint16_t)input[0] | ((uint16_t)input[1] << 8U));
}
static uint32_t Read32(const unsigned char *input)
{
    return (uint32_t)input[0] | ((uint32_t)input[1] << 8U) |
           ((uint32_t)input[2] << 16U) | ((uint32_t)input[3] << 24U);
}
static uint64_t Read64(const unsigned char *input)
{
    return (uint64_t)Read32(input) | ((uint64_t)Read32(input + 4U) << 32U);
}

/* Only a CLAIM may introduce or replace a live source owner. In a later
 * CLAIM, generation must increase; an expired token must never name the next
 * service merely because it reused an array slot. */
static UmiStatus Apply(InterruptModel *model, UmiEducationInterruptReport *report,
                       unsigned event, uint16_t sourceId, uint32_t owner,
                       uint32_t generation, uint64_t tick, uint64_t detail)
{
    if (sourceId == 0U || sourceId > UMI_EDUCATION_INTERRUPT_MAX_SOURCES ||
        owner == 0U || generation == 0U || tick < model->tick)
        return UMI_STATUS_PARSE_ERROR;
    InterruptSource *source = &model->sources[(size_t)sourceId - 1U];
    if (event == UMI_EDUCATION_INTERRUPT_CLAIM) {
        if (detail != 0U || source->stage != INTERRUPT_FREE ||
            generation <= source->lastGeneration)
            return UMI_STATUS_PARSE_ERROR;
        source->stage = INTERRUPT_LEASED;
        source->owner = owner;
        source->generation = generation;
        source->lastGeneration = generation;
        ++model->owned;
        ++report->claims;
        if (model->owned > report->peakOwned) report->peakOwned = model->owned;
        model->tick = tick;
        return UMI_STATUS_OK;
    }
    if (source->stage == INTERRUPT_FREE || source->owner != owner ||
        source->generation != generation)
        return UMI_STATUS_PARSE_ERROR;
    if (event != UMI_EDUCATION_INTERRUPT_FAULT && detail != 0U)
        return UMI_STATUS_PARSE_ERROR;
    switch (event) {
    case UMI_EDUCATION_INTERRUPT_ARM:
        if (source->stage != INTERRUPT_LEASED) return UMI_STATUS_PARSE_ERROR;
        source->stage = INTERRUPT_ARMED;
        ++report->arms;
        break;
    case UMI_EDUCATION_INTERRUPT_ENTER:
        if (source->stage != INTERRUPT_ARMED) return UMI_STATUS_PARSE_ERROR;
        source->stage = INTERRUPT_SERVICING;
        ++report->entries;
        break;
    case UMI_EDUCATION_INTERRUPT_ACK:
        if (source->stage != INTERRUPT_SERVICING) return UMI_STATUS_PARSE_ERROR;
        source->stage = INTERRUPT_ACKNOWLEDGED;
        ++report->acknowledgements;
        break;
    case UMI_EDUCATION_INTERRUPT_RETURN:
        if (source->stage != INTERRUPT_ACKNOWLEDGED) return UMI_STATUS_PARSE_ERROR;
        source->stage = INTERRUPT_LEASED;
        ++report->returns;
        break;
    case UMI_EDUCATION_INTERRUPT_CANCEL:
        if (source->stage != INTERRUPT_ARMED) return UMI_STATUS_PARSE_ERROR;
        source->stage = INTERRUPT_LEASED;
        ++report->cancellations;
        break;
    case UMI_EDUCATION_INTERRUPT_FAULT:
        if ((source->stage != INTERRUPT_SERVICING &&
             source->stage != INTERRUPT_ACKNOWLEDGED) || detail == 0U || detail > 255U)
            return UMI_STATUS_PARSE_ERROR;
        source->stage = INTERRUPT_FAULTED;
        ++report->faults;
        break;
    case UMI_EDUCATION_INTERRUPT_RECOVER:
        if (source->stage != INTERRUPT_FAULTED) return UMI_STATUS_PARSE_ERROR;
        source->stage = INTERRUPT_LEASED;
        ++report->recoveries;
        break;
    case UMI_EDUCATION_INTERRUPT_RELEASE:
        if (source->stage != INTERRUPT_LEASED) return UMI_STATUS_PARSE_ERROR;
        /* Do not erase lastGeneration. Reissue with an old generation must
         * remain a refused operation after the slot becomes free. */
        source->stage = INTERRUPT_FREE;
        source->owner = 0U;
        source->generation = 0U;
        --model->owned;
        ++report->releases;
        break;
    default:
        return UMI_STATUS_PARSE_ERROR;
    }
    model->tick = tick;
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationInterruptTraceInspect(const void *bytes, size_t length,
                                             UmiEducationInterruptReport *report)
{
    static const unsigned char MAGIC[8] = {'U','M','I','I','R','Q','1',0};
    if (bytes == NULL || report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_EDUCATION_INTERRUPT_MAX_FILE_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length < UMI_EDUCATION_INTERRUPT_HEADER_BYTES) return UMI_STATUS_PARSE_ERROR;
    const unsigned char *input = bytes;
    if (memcmp(input, MAGIC, sizeof(MAGIC)) != 0 ||
        Read16(input + 8U) != 1U || Read16(input + 10U) != 0U)
        return UMI_STATUS_PARSE_ERROR;
    const uint32_t count = Read32(input + 12U);
    if (count > UMI_EDUCATION_INTERRUPT_MAX_RECORDS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length != UMI_EDUCATION_INTERRUPT_HEADER_BYTES +
                  (size_t)count * UMI_EDUCATION_INTERRUPT_RECORD_BYTES)
        return UMI_STATUS_PARSE_ERROR;

    InterruptModel model = {0};
    UmiEducationInterruptReport next = {0};
    next.recordCount = count;
    for (uint32_t i = 0U; i < count; ++i) {
        const unsigned char *record = input + UMI_EDUCATION_INTERRUPT_HEADER_BYTES +
            (size_t)i * UMI_EDUCATION_INTERRUPT_RECORD_BYTES;
        if (Read32(record) != i + 1U || record[5] != 0U)
            return UMI_STATUS_PARSE_ERROR;
        const UmiStatus status = Apply(&model, &next, (unsigned)record[4],
            Read16(record + 6U), Read32(record + 8U), Read32(record + 12U),
            Read64(record + 16U), Read64(record + 24U));
        if (status != UMI_STATUS_OK) return status;
    }
    next.lastTick = model.tick;
    next.ownedAtEnd = model.owned;
    for (size_t i = 0U; i < UMI_EDUCATION_INTERRUPT_MAX_SOURCES; ++i) {
        const InterruptStage stage = model.sources[i].stage;
        if (stage == INTERRUPT_ARMED || stage == INTERRUPT_SERVICING ||
            stage == INTERRUPT_ACKNOWLEDGED) ++next.activeAtEnd;
        if (stage == INTERRUPT_FAULTED) ++next.faultedAtEnd;
    }
    *report = next;
    return UMI_STATUS_OK;
}

/* Retain the original file wrapper for review. The replacement stages its
 * report until reading and closing succeed, like the other shared inspectors. */
#if 0
UmiStatus UmiEducationInterruptTraceInspectFile(const char *path,
                                                 UmiEducationInterruptReport *report)
{
    if (path == NULL || *path == '\0' || report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *stream = fopen(path, "rb");
    if (stream == NULL) return UMI_STATUS_NOT_FOUND;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    if (fseek(stream, 0L, SEEK_END) == 0) {
        const long end = ftell(stream);
        if (end >= 0L && (unsigned long)end > UMI_EDUCATION_INTERRUPT_MAX_FILE_BYTES)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (end >= 0L && fseek(stream, 0L, SEEK_SET) == 0) {
            const size_t length = (size_t)end;
            unsigned char *buffer = malloc(length != 0U ? length : 1U);
            if (buffer == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                if (fread(buffer, 1U, length, stream) == length)
                    status = UmiEducationInterruptTraceInspect(buffer, length, report);
                free(buffer);
            }
        }
    }
    if (fclose(stream) != 0 && status == UMI_STATUS_OK)
        return UMI_STATUS_IO_ERROR;
    return status;
}
#endif
UmiStatus UmiEducationInterruptTraceInspectFile(const char *path,
                                                 UmiEducationInterruptReport *report)
{
    if (path == NULL || *path == '\0' || report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *stream = fopen(path, "rb");
    if (stream == NULL) return UMI_STATUS_NOT_FOUND;
    UmiEducationInterruptReport candidate = {0};
    UmiStatus status = UMI_STATUS_IO_ERROR;
    if (fseek(stream, 0L, SEEK_END) == 0) {
        const long end = ftell(stream);
        if (end >= 0L && (unsigned long)end > UMI_EDUCATION_INTERRUPT_MAX_FILE_BYTES)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (end >= 0L && fseek(stream, 0L, SEEK_SET) == 0) {
            const size_t length = (size_t)end;
            unsigned char *buffer = malloc(length != 0U ? length : 1U);
            if (buffer == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                if (fread(buffer, 1U, length, stream) == length)
                    status = UmiEducationInterruptTraceInspect(buffer, length, &candidate);
                free(buffer);
            }
        }
    }
    if (fclose(stream) != 0 && status == UMI_STATUS_OK)
        return UMI_STATUS_IO_ERROR;
    /* A late close error must not replace a report already displayed by an IDE. */
    if (status == UMI_STATUS_OK) *report = candidate;
    return status;
}
