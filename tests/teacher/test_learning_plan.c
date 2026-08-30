/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_learning_plan.c
 *
 * PURPOSE:
 *   Implement the test learning plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>
#include "umicom/teacher/learning_plan.h"
int main(void) { UmiTeacherLearningPlan q; umi_teacher_learning_plan_init(&q); if(umi_teacher_learning_plan_append(&q,"a")!=UMI_STATUS_OK) return 1; if(umi_teacher_learning_plan_append(&q,"b")!=UMI_STATUS_OK) return 2; if(strcmp(umi_teacher_learning_plan_current(&q),"a")!=0) return 3; if(umi_teacher_learning_plan_next(&q)!=UMI_STATUS_OK || strcmp(umi_teacher_learning_plan_current(&q),"b")!=0) return 4; return 0; }
