/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_time_budget.c
 *
 * PURPOSE:
 *   Implement the test time budget behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/time_budget.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherTimeBudget b; umi_teacher_time_budget_init(&b,60U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_time_budget_reserve(&b,20U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_time_budget_consume(&b,15U)!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_time_budget_remaining(&b)!=40U) return 3; return 0; }
