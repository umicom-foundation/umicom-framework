/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_loan_schedule.c
 *
 * PURPOSE:
 *   Exercise loan schedule validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/loan_schedule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingLoanSchedule v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_loan_schedule_init(&v, "sched-1", "loan-1", 12U, 100000, 8000)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_banking_loan_schedule_total_due_minor(&v)!=108000) return 2;
    return 0;
}
