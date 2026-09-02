/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_compiler_result.c
 *
 * PURPOSE:
 *   Implement the test compiler result behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/compiler_result.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherCompilerResult s; umi_teacher_compiler_result_init(&s); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_teacher_compiler_result_record(&s,82U,1)!=UMI_STATUS_OK) return 1; umi_teacher_compiler_result_set_units(&s,4U,4U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_teacher_compiler_result_completion(&s)!=100U) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (!umi_teacher_compiler_result_complete(&s)) return 3; return 0; }
