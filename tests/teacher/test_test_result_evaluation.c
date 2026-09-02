/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_test_result_evaluation.c
 *
 * PURPOSE:
 *   Implement the test test result evaluation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/test_result_evaluation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherTestResultEvaluation r; uint32_t score; umi_teacher_test_result_evaluation_init(&r); score=umi_teacher_test_result_evaluation_compute(&r,100U,80U,60U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(score!=89U) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_teacher_test_result_evaluation_passes(&r,score)) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_test_result_evaluation_configure(&r,50U,20U,20U,70U)!=UMI_STATUS_INVALID_ARGUMENT) return 3; return 0; }
