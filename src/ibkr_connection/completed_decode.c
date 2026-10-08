/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/completed_decode.c
 * PURPOSE: Correlate the completed-order stream with its single connection-owned request.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "observation_wire.h"
#include <stdio.h>
UmiStatus UmiIbkrCompletedFrame(UmiIbkrConnection *c, uint64_t kind, const unsigned char *body, size_t length,
                                uint64_t now)
{
    UmiIbkrCompletedExpire(c, now);
    if (!c->completed.snapshot.pending)
        return UmiIbkrObservationIgnore(c);
    UmiIbkrObservationFields fields = {0};
    UmiStatus status = UmiIbkrObservationFieldsOpen(body, length, 1024U, &fields);
    if (status != UMI_STATUS_OK)
        return status;
    if (kind == 102U)
    {
        if (fields.count != 1U)
            status = UMI_STATUS_PARSE_ERROR;
        else
        {
            c->completed.snapshot.pending = false;
            c->completed.snapshot.complete = true;
            c->completed.snapshot.stale = false;
            c->completed.snapshot.completedAtMilliseconds = now;
            (void)snprintf(c->completed.snapshot.message, sizeof c->completed.snapshot.message,
                           "Completed-order end marker received. The provider's available window is not "
                           "complete trading history.");
        }
    }
    else if (kind == 101U)
    {
        UmiIbkrCompletedOrder row;
        status = UmiIbkrCompletedDecode(fields.values, fields.count, c->snapshot.protocolVersion, &row);
        if (status == UMI_STATUS_OK)
            status = UmiIbkrCompletedStoreRow(c, &row, body, length, now);
    }
    else
        status = UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrObservationFieldsClose(&fields);
    return status;
}
