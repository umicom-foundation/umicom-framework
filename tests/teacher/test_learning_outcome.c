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
int main(void) { UmiTeacherLearningOutcome s; umi_teacher_learning_outcome_init(&s); if (umi_teacher_learning_outcome_record(&s,82U,1)!=UMI_STATUS_OK) return 1; umi_teacher_learning_outcome_set_units(&s,4U,4U); if (umi_teacher_learning_outcome_completion(&s)!=100U) return 2; if (!umi_teacher_learning_outcome_complete(&s)) return 3; return 0; }
