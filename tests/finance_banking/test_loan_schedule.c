/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_loan_schedule.c
 *
 * PURPOSE:
 *   Exercise loan schedule validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/loan_schedule.h"
int main(void) {
    UmiBankingLoanSchedule v;
    if(umi_banking_loan_schedule_init(&v, "sched-1", "loan-1", 12U, 100000, 8000)!=UMI_STATUS_OK) return 1;
    if(umi_banking_loan_schedule_total_due_minor(&v)!=108000) return 2;
    return 0;
}
