/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_exercise_solution_policy.c
 *
 * PURPOSE:
 *   Implement the test exercise solution policy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/exercise_solution_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherExerciseSolutionPolicy p; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_teacher_exercise_solution_policy_configure(&p,70U,1U,2U,UMI_TEACHER_LEVEL_ADVANCED,UINT32_MAX)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (!umi_teacher_exercise_solution_policy_allows(&p,80U,1U,0U,UMI_TEACHER_LEVEL_BEGINNER,UMI_TEACHER_LANGUAGE_C23)) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_teacher_exercise_solution_policy_allows(&p,50U,1U,0U,UMI_TEACHER_LEVEL_BEGINNER,UMI_TEACHER_LANGUAGE_C23)) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_teacher_exercise_solution_policy_deficit(&p,50U)!=20U) return 4; return 0; }
