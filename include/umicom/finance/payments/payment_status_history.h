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
typedef struct UmiPaymentsPaymentStatusPoint { UmiPaymentsStatus status; int64_t epoch_millis; } UmiPaymentsPaymentStatusPoint;
typedef struct UmiPaymentsPaymentStatusHistory { UmiFinancialId id; UmiFinancialId payment_id; UmiPaymentsPaymentStatusPoint items[UMI_PAYMENTS_MAX_ITEMS]; size_t count; } UmiPaymentsPaymentStatusHistory;
UmiStatus umi_payments_payment_status_history_init(UmiPaymentsPaymentStatusHistory *value,const char *id,const char *payment_id);
UmiStatus umi_payments_payment_status_history_append(UmiPaymentsPaymentStatusHistory *value,UmiPaymentsStatus status,int64_t epoch_millis);
bool umi_payments_payment_status_history_valid(const UmiPaymentsPaymentStatusHistory *value);
#ifdef __cplusplus
}
#endif
#endif
