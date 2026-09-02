/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_repair.h
 *
 * PURPOSE:
 *   Track payment exception repair progress before resubmission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_REPAIR_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_REPAIR_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment repair data shared with callers of this public contract.
 */
typedef struct UmiPaymentsPaymentRepair {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    uint32_t issue_count;
    uint32_t resolved_count;
} UmiPaymentsPaymentRepair;
/**
 * Initialise payments payment repair from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_repair_init(UmiPaymentsPaymentRepair *value,
    const char *id,
    const char *payment_id,
    uint32_t issue_count,
    uint32_t resolved_count);
/**
 * Check that payments payment repair satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_repair_valid(const UmiPaymentsPaymentRepair *value);
/**
 * Provide the payments payment repair ready operation used by this module and its client
 * applications.
 */
bool umi_payments_payment_repair_ready(const UmiPaymentsPaymentRepair *value);
#ifdef __cplusplus
}
#endif
#endif
