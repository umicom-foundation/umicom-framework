/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/completed_snapshot.c
 *
 * PURPOSE:
 *   Copy completed-order observations atomically for native and headless clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/completed_orders.h"
#include <stdlib.h>

/* Capture and rows are copied at one observation time on the connection owner
 * thread. Publishing an owned report cannot expose a half-replaced table or
 * depend on the socket remaining alive. No callbacks or transport pumping occur. */
UmiStatus UmiIbkrCompletedOrderReportCopy(const UmiIbkrConnection *connection,
    uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds,
    UmiIbkrCompletedOrderReport *outReport)
{
    if (connection == NULL || outReport == NULL || maximumAgeMilliseconds == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrCompletedOrdersSnapshot capture;
    UmiStatus status = UmiIbkrCompletedOrdersCopy(connection, nowMilliseconds,
        maximumAgeMilliseconds, &capture);
    if (status != UMI_STATUS_OK) return status;
    if (capture.count > UMI_IBKR_COMPLETED_ORDER_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the bounded report off small native event-handler stacks. Zeroed
     * unused rows also prevent a reused destination retaining another account. */
    UmiIbkrCompletedOrderReport *copy = calloc(1U, sizeof(*copy));
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    copy->capture = capture;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < capture.count; ++i)
        status = UmiIbkrCompletedOrderCopy(connection, i, nowMilliseconds,
            maximumAgeMilliseconds, &copy->rows[i]);
    if (status == UMI_STATUS_OK) *outReport = *copy;
    free(copy);
    return status;
}
