/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/order_report.c
 *
 * PURPOSE:
 *   Copy complete order observations without exposing transport-owned storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/order_recovery.h"
#include <stdlib.h>

/* A report uses one observation time and one connection owner. Callers receive
 * a complete value copy, so native publication cannot leave a partly replaced
 * table or a pointer into the transport's mutable order cache. */
UmiStatus UmiIbkrOrderReportCopy(const UmiIbkrConnection *connection,
    uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds, UmiIbkrOrderReport *outReport)
{
    if (connection == NULL || outReport == NULL || maximumAgeMilliseconds == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrOrderRecoverySnapshot capture;
    UmiStatus status = UmiIbkrOrdersCopy(connection, nowMilliseconds,
        maximumAgeMilliseconds, &capture);
    if (status != UMI_STATUS_OK) return status;
    if (capture.count > UMI_IBKR_ORDER_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the bounded but sizeable report off small native callback stacks. */
    UmiIbkrOrderReport *copy = calloc(1U, sizeof(*copy));
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    copy->capture = capture;
    status = UmiIbkrOrderIdentityCopy(connection, &copy->identity);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < capture.count; ++i)
        status = UmiIbkrOrderCopy(connection, i, nowMilliseconds,
            maximumAgeMilliseconds, &copy->rows[i]);
    if (status == UMI_STATUS_OK) *outReport = *copy;
    free(copy);
    return status;
}
