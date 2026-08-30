/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_curriculum_prerequisite.c
 *
 * PURPOSE:
 *   Implement the test curriculum prerequisite behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/curriculum_prerequisite.h"
int main(void) { UmiTeacherCurriculumPrerequisite p; if (umi_teacher_curriculum_prerequisite_configure(&p,70U,1U,2U,UMI_TEACHER_LEVEL_ADVANCED,UINT32_MAX)!=UMI_STATUS_OK) return 1; if (!umi_teacher_curriculum_prerequisite_allows(&p,80U,1U,0U,UMI_TEACHER_LEVEL_BEGINNER,UMI_TEACHER_LANGUAGE_C23)) return 2; if (umi_teacher_curriculum_prerequisite_allows(&p,50U,1U,0U,UMI_TEACHER_LEVEL_BEGINNER,UMI_TEACHER_LANGUAGE_C23)) return 3; if (umi_teacher_curriculum_prerequisite_deficit(&p,50U)!=20U) return 4; return 0; }
