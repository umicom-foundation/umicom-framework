/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_learning_outcome.c
 *
 * PURPOSE:
 *   Implement the test learning outcome behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_outcome.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherLearningOutcome s; umi_teacher_learning_outcome_init(&s); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_teacher_learning_outcome_record(&s,82U,1)!=UMI_STATUS_OK) return 1; umi_teacher_learning_outcome_set_units(&s,4U,4U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_teacher_learning_outcome_completion(&s)!=100U) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (!umi_teacher_learning_outcome_complete(&s)) return 3; return 0; }
