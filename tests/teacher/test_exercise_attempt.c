/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_exercise_attempt.c
 *
 * PURPOSE:
 *   Implement the test exercise attempt behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/exercise_attempt.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherExerciseAttempt a; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_exercise_attempt_begin(&a,"exercise",1U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_exercise_attempt_finish(&a,88U,2U,120U,70U)!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!a.passed) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_exercise_attempt_effective_score(&a,3U)!=82U) return 4; return 0; }
