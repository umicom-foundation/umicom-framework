/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_assessment_descriptor.c
 *
 * PURPOSE:
 *   Implement the test assessment descriptor behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/assessment_descriptor.h"
int main(void) {
    UmiTeacherAssessmentDescriptor value;
    if (umi_teacher_assessment_descriptor_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    if (umi_teacher_assessment_descriptor_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_teacher_assessment_descriptor_priority(&value, 60U) != 70U) return 3;
    return 0;
}
