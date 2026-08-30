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
int main(void) { UmiTeacherCurriculumOrdering q; umi_teacher_curriculum_ordering_init(&q); if(umi_teacher_curriculum_ordering_append(&q,"a")!=UMI_STATUS_OK) return 1; if(umi_teacher_curriculum_ordering_append(&q,"b")!=UMI_STATUS_OK) return 2; if(strcmp(umi_teacher_curriculum_ordering_current(&q),"a")!=0) return 3; if(umi_teacher_curriculum_ordering_next(&q)!=UMI_STATUS_OK || strcmp(umi_teacher_curriculum_ordering_current(&q),"b")!=0) return 4; return 0; }
