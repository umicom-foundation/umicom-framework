/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/device_io_trace.c
 * PURPOSE:
 *   Validate copied device I/O requests, nonterminal cancellation intent,
 *   FIFO completion collection and quiescent device release. This read-only
 *   state machine neither performs device I/O nor authorises a Kernel request.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/device_io_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum DeviceState {
    DEVICE_EMPTY = 0,
    DEVICE_READY,
    DEVICE_QUIESCED
} DeviceState;
typedef enum RequestState {
    REQUEST_EMPTY = 0,
    REQUEST_PENDING,
    REQUEST_COMPLETED
} RequestState;
typedef struct DeviceRecord {
    DeviceState state;
    uint32_t owner;
    uint32_t generation;
    uint32_t lastGeneration;
} DeviceRecord;
typedef struct RequestRecord {
    RequestState state;
    uint32_t identity;
    uint32_t deviceSlot;
    uint32_t generation;
    uint32_t requestedBytes;
    uint32_t completionOrdinal;
    int cancelRequested;
} RequestRecord;
typedef struct IoState {
    DeviceRecord devices[UMI_EDUCATION_DEVICE_IO_MAX_DEVICES];
    RequestRecord requests[UMI_EDUCATION_DEVICE_IO_MAX_REQUESTS];
    uint32_t lastRequest;
    uint32_t outstanding;
    uint32_t readyCompletions;
    uint32_t lastCompletionOrdinal;
    uint32_t liveDevices;
    uint64_t tick;
} IoState;

static uint16_t Read16(const unsigned char *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8U));
}
static uint32_t Read32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8U) |
           ((uint32_t)p[2] << 16U) | ((uint32_t)p[3] << 24U);
}
static uint64_t Read64(const unsigned char *p)
{
    return (uint64_t)Read32(p) | ((uint64_t)Read32(p + 4U) << 32U);
}
static RequestRecord *FindRequest(IoState *state, uint32_t identity,
                                  uint32_t slot, uint32_t generation)
{
    for (size_t i = 0U; i < UMI_EDUCATION_DEVICE_IO_MAX_REQUESTS; ++i) {
        RequestRecord *request = &state->requests[i];
        if (request->state != REQUEST_EMPTY && request->identity == identity &&
            request->deviceSlot == slot && request->generation == generation)
            return request;
    }
    return NULL;
}
static RequestRecord *FreeRequest(IoState *state)
{
    for (size_t i = 0U; i < UMI_EDUCATION_DEVICE_IO_MAX_REQUESTS; ++i)
        if (state->requests[i].state == REQUEST_EMPTY) return &state->requests[i];
    return NULL;
}
/* REAP must consume the next completion in acceptance order. Submission
 * order may differ because devices finish requests at different times. */
static int OldestCompletion(const IoState *state, uint32_t ordinal)
{
    for (size_t i = 0U; i < UMI_EDUCATION_DEVICE_IO_MAX_REQUESTS; ++i) {
        const RequestRecord *request = &state->requests[i];
        if (request->state == REQUEST_COMPLETED && request->completionOrdinal < ordinal)
            return 0;
    }
    return 1;
}
static int DeviceHasRequests(const IoState *state, uint32_t slot, uint32_t generation)
{
    for (size_t i = 0U; i < UMI_EDUCATION_DEVICE_IO_MAX_REQUESTS; ++i) {
        const RequestRecord *request = &state->requests[i];
        if (request->state != REQUEST_EMPTY && request->deviceSlot == slot &&
            request->generation == generation) return 1;
    }
    return 0;
}

static UmiStatus Apply(IoState *state, UmiEducationDeviceIoReport *report,
                       const unsigned char *record)
{
    const uint8_t kind = record[4U], outcome = record[5U];
    const uint32_t slot = Read32(record + 8U);
    const uint32_t owner = Read32(record + 12U);
    const uint32_t generation = Read32(record + 16U);
    const uint32_t requestId = Read32(record + 20U);
    const uint32_t byteCount = Read32(record + 24U);
    const uint64_t tick = Read64(record + 28U);
    if (slot == 0U || slot > UMI_EDUCATION_DEVICE_IO_MAX_DEVICES ||
        owner == 0U || generation == 0U || tick < state->tick ||
        outcome > UMI_EDUCATION_DEVICE_IO_CANCELLED || Read16(record + 6U) != 0U ||
        Read32(record + 36U) != 0U)
        return UMI_STATUS_PARSE_ERROR;
    DeviceRecord *device = &state->devices[(size_t)slot - 1U];
    if (kind == UMI_EDUCATION_DEVICE_IO_ATTACH) {
        if (outcome != 0U || requestId != 0U || byteCount != 0U ||
            device->state != DEVICE_EMPTY || generation <= device->lastGeneration)
            return UMI_STATUS_PARSE_ERROR;
        device->state = DEVICE_READY;
        device->owner = owner;
        device->generation = generation;
        device->lastGeneration = generation;
        ++report->attachments;
        ++state->liveDevices;
        state->tick = tick;
        return UMI_STATUS_OK;
    }
    if (device->state == DEVICE_EMPTY || device->owner != owner ||
        device->generation != generation)
        return UMI_STATUS_PARSE_ERROR;
    /* Outcome belongs ONLY to terminal COMPLETE. Treat it as a result of the
     * device, not a capability held by a caller requesting cancellation. */
    if (kind != UMI_EDUCATION_DEVICE_IO_COMPLETE && outcome != 0U)
        return UMI_STATUS_PARSE_ERROR;
    switch (kind) {
    case UMI_EDUCATION_DEVICE_IO_SUBMIT: {
        if (device->state != DEVICE_READY || requestId == 0U ||
            requestId <= state->lastRequest || byteCount == 0U ||
            byteCount > UMI_EDUCATION_DEVICE_IO_MAX_TRANSFER_BYTES ||
            state->outstanding >= UMI_EDUCATION_DEVICE_IO_MAX_REQUESTS)
            return UMI_STATUS_PARSE_ERROR;
        RequestRecord *freeSlot = FreeRequest(state);
        if (freeSlot == NULL) return UMI_STATUS_PARSE_ERROR;
        *freeSlot = (RequestRecord){.state=REQUEST_PENDING, .identity=requestId,
            .deviceSlot=slot, .generation=generation, .requestedBytes=byteCount};
        state->lastRequest = requestId;
        ++state->outstanding;
        ++report->submissions;
        report->bytesRequested += byteCount;
        if (report->peakOutstanding < state->outstanding)
            report->peakOutstanding = state->outstanding;
        break;
    }
    case UMI_EDUCATION_DEVICE_IO_CANCEL_REQUEST: {
        if (requestId == 0U || byteCount != 0U) return UMI_STATUS_PARSE_ERROR;
        RequestRecord *request = FindRequest(state, requestId, slot, generation);
        if (request == NULL || request->state != REQUEST_PENDING || request->cancelRequested)
            return UMI_STATUS_PARSE_ERROR;
        request->cancelRequested = 1;
        ++report->cancellationRequests;
        break;
    }
    case UMI_EDUCATION_DEVICE_IO_COMPLETE: {
        if (requestId == 0U || state->readyCompletions >=
            UMI_EDUCATION_DEVICE_IO_COMPLETION_CAPACITY)
            return UMI_STATUS_PARSE_ERROR;
        RequestRecord *request = FindRequest(state, requestId, slot, generation);
        if (request == NULL || request->state != REQUEST_PENDING ||
            byteCount > request->requestedBytes ||
            (outcome != UMI_EDUCATION_DEVICE_IO_SUCCESS && byteCount != 0U) ||
            (outcome == UMI_EDUCATION_DEVICE_IO_CANCELLED && !request->cancelRequested))
            return UMI_STATUS_PARSE_ERROR;
        /* The device may win a race against an earlier cancellation request.
         * The result is authoritative; cancellation intent alone is not. */
        request->state = REQUEST_COMPLETED;
        request->completionOrdinal = ++state->lastCompletionOrdinal;
        ++state->readyCompletions;
        ++report->completions;
        if (outcome == UMI_EDUCATION_DEVICE_IO_SUCCESS) ++report->succeeded;
        else if (outcome == UMI_EDUCATION_DEVICE_IO_FAILURE) ++report->failed;
        else ++report->cancellationAcknowledgements;
        report->bytesCompleted += byteCount;
        if (report->peakReadyCompletions < state->readyCompletions)
            report->peakReadyCompletions = state->readyCompletions;
        break;
    }
    case UMI_EDUCATION_DEVICE_IO_REAP: {
        if (requestId == 0U || byteCount != 0U) return UMI_STATUS_PARSE_ERROR;
        RequestRecord *request = FindRequest(state, requestId, slot, generation);
        if (request == NULL || request->state != REQUEST_COMPLETED ||
            !OldestCompletion(state, request->completionOrdinal))
            return UMI_STATUS_PARSE_ERROR;
        *request = (RequestRecord){0};
        --state->outstanding;
        --state->readyCompletions;
        ++report->reaped;
        break;
    }
    case UMI_EDUCATION_DEVICE_IO_QUIESCE:
        if (requestId != 0U || byteCount != 0U || device->state != DEVICE_READY)
            return UMI_STATUS_PARSE_ERROR;
        device->state = DEVICE_QUIESCED;
        ++report->quiesced;
        break;
    case UMI_EDUCATION_DEVICE_IO_RESUME:
        if (requestId != 0U || byteCount != 0U || device->state != DEVICE_QUIESCED)
            return UMI_STATUS_PARSE_ERROR;
        device->state = DEVICE_READY;
        ++report->resumed;
        break;
    case UMI_EDUCATION_DEVICE_IO_DETACH:
        if (requestId != 0U || byteCount != 0U || device->state != DEVICE_QUIESCED ||
            DeviceHasRequests(state, slot, generation)) return UMI_STATUS_PARSE_ERROR;
        device->state = DEVICE_EMPTY;
        device->owner = 0U;
        device->generation = 0U;
        --state->liveDevices;
        ++report->detached;
        break;
    default: return UMI_STATUS_PARSE_ERROR;
    }
    state->tick = tick;
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationDeviceIoTraceInspect(const void *bytes, size_t length,
                                            UmiEducationDeviceIoReport *report)
{
    static const unsigned char MAGIC[8] = {'U','M','I','D','I','O','1',0};
    if (bytes == NULL || report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_EDUCATION_DEVICE_IO_MAX_FILE_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length < UMI_EDUCATION_DEVICE_IO_HEADER_BYTES) return UMI_STATUS_PARSE_ERROR;
    const unsigned char *input = bytes;
    if (memcmp(input, MAGIC, sizeof(MAGIC)) != 0 ||
        Read16(input + 8U) != 1U || Read16(input + 10U) != 0U ||
        Read32(input + 16U) != 0U || Read32(input + 20U) != 0U)
        return UMI_STATUS_PARSE_ERROR;
    const uint32_t count = Read32(input + 12U);
    if (count > UMI_EDUCATION_DEVICE_IO_MAX_RECORDS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length != UMI_EDUCATION_DEVICE_IO_HEADER_BYTES +
                  (size_t)count * UMI_EDUCATION_DEVICE_IO_RECORD_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    IoState state = {0};
    UmiEducationDeviceIoReport next = {0};
    next.recordCount = count;
    for (uint32_t i = 0U; i < count; ++i) {
        const unsigned char *record = input + UMI_EDUCATION_DEVICE_IO_HEADER_BYTES +
            (size_t)i * UMI_EDUCATION_DEVICE_IO_RECORD_BYTES;
        if (Read32(record) != i + 1U) return UMI_STATUS_PARSE_ERROR;
        if (Apply(&state, &next, record) != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    }
    next.outstandingAtEnd = state.outstanding;
    next.readyCompletionsAtEnd = state.readyCompletions;
    next.liveDevicesAtEnd = state.liveDevices;
    next.lastTick = state.tick;
    *report = next;
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationDeviceIoTraceInspectFile(const char *path,
                                                UmiEducationDeviceIoReport *report)
{
    if (path == NULL || path[0] == '\0' || report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *stream = fopen(path, "rb");
    if (stream == NULL) return UMI_STATUS_NOT_FOUND;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    UmiEducationDeviceIoReport pending;
    if (fseek(stream, 0L, SEEK_END) == 0) {
        const long end = ftell(stream);
        if (end >= 0L && (unsigned long)end > UMI_EDUCATION_DEVICE_IO_MAX_FILE_BYTES)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (end >= 0L && fseek(stream, 0L, SEEK_SET) == 0) {
            const size_t length = (size_t)end;
            unsigned char *buffer = malloc(length != 0U ? length : 1U);
            if (buffer == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                if (fread(buffer, 1U, length, stream) == length)
                    status = UmiEducationDeviceIoTraceInspect(buffer, length, &pending);
                free(buffer);
            }
        }
    }
    if (fclose(stream) != 0) return UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK) *report = pending;
    return status;
}
