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
typedef struct UmiBankingLoanSchedule {
    UmiFinancialId id;
    UmiFinancialId loan_id;
    uint32_t installment_count;
    int64_t principal_minor;
    int64_t interest_minor;
} UmiBankingLoanSchedule;
UmiStatus umi_banking_loan_schedule_init(UmiBankingLoanSchedule *value,
    const char *id,
    const char *loan_id,
    uint32_t installment_count,
    int64_t principal_minor,
    int64_t interest_minor);
bool umi_banking_loan_schedule_valid(const UmiBankingLoanSchedule *value);
int64_t umi_banking_loan_schedule_total_due_minor(const UmiBankingLoanSchedule *value);
#ifdef __cplusplus
}
#endif
#endif
