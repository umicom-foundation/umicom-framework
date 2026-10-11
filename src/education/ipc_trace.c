/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/ipc_trace.c
 * PURPOSE: Validate a small educational IPC event stream with explicit copy,
 *          FIFO ownership and bounded input; never communicate with a kernel.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/ipc_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The inspector models only who owns an outstanding message, not its payload.
 * Real Umicom Kernel message channels own actual bytes and protected handles;
 * they are authoritative for genuine execution and permission decisions. */
typedef struct UmiEducationPendingMessage {
    uint32_t sender;
    uint32_t recipient;
} UmiEducationPendingMessage;

static uint16_t Read16(const unsigned char *bytes)
{
    return (uint16_t)((uint16_t)bytes[0] | (uint16_t)((uint16_t)bytes[1] << 8U));
}

static uint32_t Read32(const unsigned char *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8U) |
           ((uint32_t)bytes[2] << 16U) | ((uint32_t)bytes[3] << 24U);
}

UmiStatus UmiEducationIpcTraceInspect(const void *data, size_t byteCount,
                                      UmiEducationIpcTraceReport *report)
{
    static const unsigned char MAGIC[8] = {'U','M','I','I','P','C','1',0};
    if (data == NULL || report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (byteCount > UMI_EDUCATION_IPC_TRACE_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (byteCount < UMI_EDUCATION_IPC_TRACE_HEADER_BYTES)
        return UMI_STATUS_PARSE_ERROR;

    const unsigned char *const bytes = data;
    if (memcmp(bytes, MAGIC, sizeof(MAGIC)) != 0 ||
        Read16(bytes + 8U) != 1U || Read16(bytes + 10U) != 0U)
        return UMI_STATUS_PARSE_ERROR;

    const uint32_t recordCount = Read32(bytes + 12U);
    if (recordCount > UMI_EDUCATION_IPC_TRACE_MAX_RECORDS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Every declared record requires a complete 16-byte header. This check
     * also prevents excessive iterations on a tiny, forged input. */
    if ((size_t)recordCount >
        (byteCount - UMI_EDUCATION_IPC_TRACE_HEADER_BYTES) /
            UMI_EDUCATION_IPC_TRACE_RECORD_BYTES)
        return UMI_STATUS_PARSE_ERROR;

    UmiEducationPendingMessage queue[UMI_EDUCATION_IPC_TRACE_MAX_PENDING] = {{0}};
    UmiEducationIpcTraceReport pending = {0};
    pending.recordCount = recordCount;
    size_t offset = UMI_EDUCATION_IPC_TRACE_HEADER_BYTES;
    uint32_t head = 0U;
    uint32_t count = 0U;

    for (uint32_t index = 0U; index < recordCount; ++index) {
        if (byteCount - offset < UMI_EDUCATION_IPC_TRACE_RECORD_BYTES)
            return UMI_STATUS_PARSE_ERROR;
        const unsigned char *const record = bytes + offset;
        const uint32_t ordinal = Read32(record);
        const unsigned kind = record[4];
        const unsigned flags = record[5];
        const uint16_t payloadLength = Read16(record + 6U);
        const uint32_t sender = Read32(record + 8U);
        const uint32_t recipient = Read32(record + 12U);
        offset += UMI_EDUCATION_IPC_TRACE_RECORD_BYTES;
        if (ordinal != index + 1U || flags != 0U || sender == 0U ||
            recipient == 0U || sender == recipient ||
            payloadLength > UMI_EDUCATION_IPC_TRACE_MAX_PAYLOAD ||
            (size_t)payloadLength > byteCount - offset)
            return UMI_STATUS_PARSE_ERROR;

        if (kind == 1U) { /* SEND: queue owns a copy after acceptance. */
            if (payloadLength == 0U)
                return UMI_STATUS_PARSE_ERROR;
            if (count >= UMI_EDUCATION_IPC_TRACE_MAX_PENDING)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            const uint32_t slot = (head + count) % UMI_EDUCATION_IPC_TRACE_MAX_PENDING;
            queue[slot].sender = sender;
            queue[slot].recipient = recipient;
            ++count;
            ++pending.sendCount;
            pending.payloadBytes += payloadLength;
            if (count > pending.highWater)
                pending.highWater = count;
        } else if (kind == 2U) { /* RECEIVE: consume only the oldest matching send. */
            if (payloadLength != 0U || count == 0U ||
                queue[head].sender != sender || queue[head].recipient != recipient)
                return UMI_STATUS_PARSE_ERROR;
            queue[head] = (UmiEducationPendingMessage){0};
            head = (head + 1U) % UMI_EDUCATION_IPC_TRACE_MAX_PENDING;
            --count;
            ++pending.receiveCount;
        } else {
            return UMI_STATUS_PARSE_ERROR;
        }
        offset += (size_t)payloadLength;
    }
    /* Reject hidden trailing records or bytes after the advertised count. */
    if (offset != byteCount)
        return UMI_STATUS_PARSE_ERROR;
    pending.pendingCount = count;
    *report = pending;
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationIpcTraceInspectFile(const char *path,
                                          UmiEducationIpcTraceReport *report)
{
    if (path == NULL || path[0] == '\0' || report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *input = fopen(path, "rb");
    if (input == NULL)
        return UMI_STATUS_IO_ERROR;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    UmiEducationIpcTraceReport candidate = {0};
    unsigned char *bytes = NULL;
    do {
        if (fseek(input, 0L, SEEK_END) != 0)
            break;
        const long end = ftell(input);
        if (end < 0L)
            break;
        if ((unsigned long long)end > (unsigned long long)UMI_EDUCATION_IPC_TRACE_MAX_BYTES) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        if (fseek(input, 0L, SEEK_SET) != 0)
            break;
        const size_t size = (size_t)end;
        bytes = malloc(size == 0U ? 1U : size);
        if (bytes == NULL) {
            status = UMI_STATUS_OUT_OF_MEMORY;
            break;
        }
        if (fread(bytes, 1U, size, input) != size)
            break;
        /* A changing file must not silently bypass the size cap or add bytes
         * to a supposedly complete trace. */
        if (fgetc(input) != EOF || ferror(input) != 0)
            break;
        status = UmiEducationIpcTraceInspect(bytes, size, &candidate);
    } while (0);
    free(bytes);
    if (fclose(input) != 0 && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK)
        *report = candidate;
    return status;
}
