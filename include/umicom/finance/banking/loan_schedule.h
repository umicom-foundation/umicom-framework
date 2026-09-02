/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/loan_schedule.h
 *
 * PURPOSE:
 *   Summarise amortising loan schedule principal, interest and installment count.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_LOAN_SCHEDULE_H
#define UMICOM_FINANCE_BANKING_LOAN_SCHEDULE_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking loan schedule data shared with callers of this public contract.
 */
typedef struct UmiBankingLoanSchedule {
    UmiFinancialId id;
    UmiFinancialId loan_id;
    uint32_t installment_count;
    int64_t principal_minor;
    int64_t interest_minor;
} UmiBankingLoanSchedule;
/**
 * Initialise banking loan schedule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_loan_schedule_init(UmiBankingLoanSchedule *value,
    const char *id,
    const char *loan_id,
    uint32_t installment_count,
    int64_t principal_minor,
    int64_t interest_minor);
/**
 * Check that banking loan schedule satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_loan_schedule_valid(const UmiBankingLoanSchedule *value);
/**
 * Provide the banking loan schedule total due minor operation used by this module and its
 * client applications.
 */
int64_t umi_banking_loan_schedule_total_due_minor(const UmiBankingLoanSchedule *value);
#ifdef __cplusplus
}
#endif
#endif
