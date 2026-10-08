/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/observation_wire.c
 * PURPOSE: Split bounded broker frames without exposing packet storage to callers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "observation_wire.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiIbkrObservationFieldsOpen(const unsigned char *body, size_t length, size_t maximum,
                                       UmiIbkrObservationFields *out)
{
    /* A bounded historical response carries eight fields per candle. Retain
     * the earlier global field cap for review; each decoder still supplies its
     * own smaller limit and the complete frame remains capped at 64 KiB. */
#if 0
    if (body == NULL || out == NULL || length == 0U || length > UMI_IBKR_FRAME_LIMIT || maximum == 0U ||
        maximum > 1024U)
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    if (body == NULL || out == NULL || length == 0U || length > UMI_IBKR_FRAME_LIMIT || maximum == 0U ||
        maximum > 8192U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (body[length - 1U] != 0U)
        return UMI_STATUS_PARSE_ERROR;
    size_t count = 0U;
    for (size_t i = 0U; i < length; ++i)
        if (body[i] == 0U)
            ++count;
    if (count > maximum)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiIbkrObservationFields fields = {0};
    fields.storage = malloc(length);
    fields.values = calloc(count, sizeof *fields.values);
    if (fields.storage == NULL || fields.values == NULL)
    {
        UmiIbkrObservationFieldsClose(&fields);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(fields.storage, body, length);
    size_t start = 0U;
    for (size_t i = 0U; i < length; ++i)
        if (body[i] == 0U)
        {
            fields.values[fields.count++] = fields.storage + start;
            start = i + 1U;
        }
    *out = fields;
    return UMI_STATUS_OK;
}
void UmiIbkrObservationFieldsClose(UmiIbkrObservationFields *fields)
{
    if (fields == NULL)
        return;
    free(fields->storage);
    free(fields->values);
    memset(fields, 0, sizeof *fields);
}
UmiStatus UmiIbkrObservationIgnore(UmiIbkrConnection *connection)
{
    if (connection->snapshot.ignoredFrames == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++connection->snapshot.ignoredFrames;
    return UMI_STATUS_OK;
}
