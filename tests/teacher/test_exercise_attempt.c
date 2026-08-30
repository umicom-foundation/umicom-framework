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
int main(void) { UmiTeacherExerciseAttempt a; if(umi_teacher_exercise_attempt_begin(&a,"exercise",1U)!=UMI_STATUS_OK) return 1; if(umi_teacher_exercise_attempt_finish(&a,88U,2U,120U,70U)!=UMI_STATUS_OK) return 2; if(!a.passed) return 3; if(umi_teacher_exercise_attempt_effective_score(&a,3U)!=82U) return 4; return 0; }
