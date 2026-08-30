/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_assessment_rubric.c
 *
 * PURPOSE:
 *   Implement the test assessment rubric behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/assessment_rubric.h"
int main(void) { UmiTeacherAssessmentRubric r; uint32_t score; umi_teacher_assessment_rubric_init(&r); score=umi_teacher_assessment_rubric_compute(&r,100U,80U,60U); if(score!=89U) return 1; if(!umi_teacher_assessment_rubric_passes(&r,score)) return 2; if(umi_teacher_assessment_rubric_configure(&r,50U,20U,20U,70U)!=UMI_STATUS_INVALID_ARGUMENT) return 3; return 0; }
