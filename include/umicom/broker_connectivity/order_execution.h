/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/order_execution.h
 *
 * PURPOSE:
 *   Review and submit explicitly authorised single-instrument broker limit orders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_ORDER_EXECUTION_H
#define UMICOM_IBKR_ORDER_EXECUTION_H
#include "umicom/broker_connectivity/contract_details.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_IBKR_SUBMISSION_LIMIT 32U
typedef struct UmiIbkrOrderDraft {
    uint32_t contractId, quantity;
    char exchange[64], account[64], limitPrice[48];
    bool sell;
} UmiIbkrOrderDraft;
typedef struct UmiIbkrOrderReview {
    uint64_t token, createdAtMilliseconds;
    UmiIbkrOrderDraft draft;
    UmiIbkrContractDescription instrument;
    UmiTradingEnvironment requestedEnvironment;
} UmiIbkrOrderReview;
typedef struct UmiIbkrOrderSubmission {
    uint32_t orderId;
    UmiIbkrOrderDraft draft;
    bool cancelRequested, brokerObserved, rejected, cancelled, filled, uncertain;
    int providerCode;
    char brokerStatus[64], message[512];
    uint64_t submittedAtMilliseconds;
} UmiIbkrOrderSubmission;
typedef struct UmiIbkrExecutionReport {
    bool enabled, reviewPending;
    char account[64];
    size_t count;
    UmiIbkrOrderSubmission orders[UMI_IBKR_SUBMISSION_LIMIT];
} UmiIbkrExecutionReport;
/* Single connection owner, like Pump. Enabling is explicit and never persisted.
 * The selected account must have complete balance and position observations.
 * confirmedBrokerMode means the user checked the Gateway login/account: a port,
 * account prefix or requested Paper setting cannot attest the actual server.
 * This switch cannot override the broker's own read-only API configuration. */
UmiStatus UmiIbkrExecutionEnable(UmiIbkrConnection *, const char *account,
    UmiTradingEnvironment requestedMode, bool confirmedBrokerMode);
void UmiIbkrExecutionDisable(UmiIbkrConnection *);
/* A review uses one complete exact description no older than 60 seconds.
 * Initial execution scope: whole quantities of STK/FUT, DAY limit orders during
 * regular hours. No modification, market orders, brackets, bonds or allocation.
 * Review expires after 30 seconds. Submission consumes it once; it never retries.
 * Queue success means pending delivery, not broker acceptance or a fill. */
UmiStatus UmiIbkrOrderPrepare(UmiIbkrConnection *, const UmiIbkrOrderDraft *,
    uint64_t nowMilliseconds, UmiIbkrOrderReview *);
UmiStatus UmiIbkrOrderSubmit(UmiIbkrConnection *, uint64_t expectedToken,
    uint64_t nowMilliseconds, uint32_t *outOrderId);
UmiStatus UmiIbkrOrderCancelSubmitted(UmiIbkrConnection *, uint32_t orderId,
    uint64_t nowMilliseconds);
UmiStatus UmiIbkrExecutionCopy(const UmiIbkrConnection *, UmiIbkrExecutionReport *);
#ifdef __cplusplus
}
#endif
#endif
