/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_curriculum_ordering.c
 *
 * PURPOSE:
 *   Implement the test curriculum ordering behavior for
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
#include "umicom/teacher/curriculum_ordering.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherCurriculumOrdering q; umi_teacher_curriculum_ordering_init(&q); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_curriculum_ordering_append(&q,"a")!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_curriculum_ordering_append(&q,"b")!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(umi_teacher_curriculum_ordering_current(&q),"a")!=0) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_curriculum_ordering_next(&q)!=UMI_STATUS_OK || strcmp(umi_teacher_curriculum_ordering_current(&q),"b")!=0) return 4; return 0; }
