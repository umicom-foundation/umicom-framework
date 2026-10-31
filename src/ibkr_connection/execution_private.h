/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/execution_private.h
 *
 * PURPOSE:
 *   Keep reviewed tickets and submission evidence inside the connection owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_EXECUTION_PRIVATE_H
#define UMICOM_IBKR_EXECUTION_PRIVATE_H
#include "umicom/broker_connectivity/order_execution.h"
typedef struct UmiIbkrExecutionStore {
    UmiIbkrExecutionReport report;
    UmiIbkrOrderReview review;
    uint64_t nextToken;
} UmiIbkrExecutionStore;
UmiStatus UmiIbkrOrderWire(UmiIbkrConnection *, const UmiIbkrOrderReview *, uint32_t);
UmiStatus UmiIbkrOrderCancelWire(UmiIbkrConnection *, uint32_t);
void UmiIbkrSubmissionObserve(UmiIbkrConnection *, const UmiIbkrRecoveredOrder *);
bool UmiIbkrSubmissionMessage(UmiIbkrConnection *, const char *, int, const char *);
void UmiIbkrSubmissionClosed(UmiIbkrConnection *);
#endif
