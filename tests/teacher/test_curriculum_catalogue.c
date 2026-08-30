/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_curriculum_catalogue.c
 *
 * PURPOSE:
 *   Implement the test curriculum catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/curriculum_catalogue.h"
int main(void) { UmiTeacherCurriculumCatalogue c; umi_teacher_curriculum_catalogue_init(&c); if (umi_teacher_curriculum_catalogue_upsert(&c,"one",UMI_TEACHER_LANGUAGE_C23,UMI_TEACHER_LEVEL_BEGINNER,4U)!=UMI_STATUS_OK) return 1; if (umi_teacher_curriculum_catalogue_find(&c,"one")==0) return 2; if (umi_teacher_curriculum_catalogue_eligible_count(&c,UMI_TEACHER_LANGUAGE_C23,UMI_TEACHER_LEVEL_BEGINNER)!=1U) return 3; return 0; }
