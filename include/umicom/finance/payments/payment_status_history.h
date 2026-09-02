/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_status_history.h
 *
 * PURPOSE:
 *   Maintain monotonic bounded payment status history for audit and exception handling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_STATUS_HISTORY_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_STATUS_HISTORY_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment status point data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsPaymentStatusPoint { UmiPaymentsStatus status; int64_t epoch_millis; } UmiPaymentsPaymentStatusPoint;
/**
 * Represent the payments payment status history data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsPaymentStatusHistory { UmiFinancialId id; UmiFinancialId payment_id; UmiPaymentsPaymentStatusPoint items[UMI_PAYMENTS_MAX_ITEMS]; size_t count; } UmiPaymentsPaymentStatusHistory;
/**
 * Initialise payments payment status history from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_payments_payment_status_history_init(UmiPaymentsPaymentStatusHistory *value,const char *id,const char *payment_id);
/**
 * Add payments payment status history only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_payments_payment_status_history_append(UmiPaymentsPaymentStatusHistory *value,UmiPaymentsStatus status,int64_t epoch_millis);
/**
 * Check that payments payment status history satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_status_history_valid(const UmiPaymentsPaymentStatusHistory *value);
#ifdef __cplusplus
}
#endif
#endif
