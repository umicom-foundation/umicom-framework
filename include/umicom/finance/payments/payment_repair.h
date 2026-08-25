/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_repair.h
 *
 * PURPOSE:
 *   Track payment exception repair progress before resubmission.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_REPAIR_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_REPAIR_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentRepair {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    uint32_t issue_count;
    uint32_t resolved_count;
} UmiPaymentsPaymentRepair;
UmiStatus umi_payments_payment_repair_init(UmiPaymentsPaymentRepair *value,
    const char *id,
    const char *payment_id,
    uint32_t issue_count,
    uint32_t resolved_count);
bool umi_payments_payment_repair_valid(const UmiPaymentsPaymentRepair *value);
bool umi_payments_payment_repair_ready(const UmiPaymentsPaymentRepair *value);
#ifdef __cplusplus
}
#endif
#endif
