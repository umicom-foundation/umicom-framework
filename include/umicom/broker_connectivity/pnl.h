/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/pnl.h
 * PURPOSE: Observe account and instrument profit and loss without changing account state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_PNL_H
#define UMICOM_BROKER_CONNECTIVITY_PNL_H
#include "umicom/broker_connectivity/connection.h"
#include "umicom/finance/decimal.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_PNL_CAPACITY 16U
    typedef struct UmiIbkrPnlSelection
    {
        char account[64], modelCode[64];
        uint32_t contractId; /* Zero selects account P&L; positive selects an instrument. */
    } UmiIbkrPnlSelection;
    typedef struct UmiIbkrPnlNumber
    {
        char reportedText[96];
        UmiDecimal value;
        bool exact; /* False for unset or unrepresentable values; never substitute zero. */
    } UmiIbkrPnlNumber;
    typedef struct UmiIbkrPnlSnapshot
    {
        uint32_t requestId;
        UmiIbkrPnlSelection selection;
        UmiIbkrPnlNumber daily, unrealized, realized, position, marketValue;
        bool subscribed, received, failed, stale;
        uint64_t requestedAtMilliseconds, receivedAtMilliseconds, revision;
        int providerCode;
        char message[256];
    } UmiIbkrPnlSnapshot;
    /* Subscribe only to an account advertised by this connection; "All" is not
 * expanded implicitly. Model code is optional. Instrument positions may be
 * negative. P&L follows the provider reset schedule, not a locally inferred day.
 * The callback has no currency field, so this API does not attach one or sum
 * streams. Up to 16 independent scopes can coexist; identical scopes are refused.
 * No order, account transfer or configuration mutation is performed. */
    UmiStatus UmiIbkrPnlSubscribe(UmiIbkrConnection *connection, const UmiIbkrPnlSelection *selection,
                                  uint64_t nowMilliseconds, uint32_t *outRequestId);
    /* Cancel only this P&L stream. Queue failure leaves it subscribed for retry.
 * Its retained snapshot becomes stale; the request ID is never reused. */
    UmiStatus UmiIbkrPnlCancel(UmiIbkrConnection *connection, uint32_t requestId);
    /* Copy retains raw provider numeric text and exact decimals when representable.
 * maximumAgeMilliseconds must be positive. Missing, old, cancelled or disconnected
 * data is stale; heartbeat traffic cannot freshen a P&L value. */
    UmiStatus UmiIbkrPnlCopy(const UmiIbkrConnection *connection, uint32_t requestId,
                             uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds,
                             UmiIbkrPnlSnapshot *out);
#ifdef __cplusplus
}
#endif
#endif
