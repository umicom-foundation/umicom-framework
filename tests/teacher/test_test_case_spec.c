/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_test_case_spec.c
 *
 * PURPOSE:
 *   Implement the test test case spec behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/test_case_spec.h"
int main(void) {
    UmiTeacherTestCaseSpec value;
    if (umi_teacher_test_case_spec_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    if (umi_teacher_test_case_spec_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_teacher_test_case_spec_priority(&value, 60U) != 70U) return 3;
    return 0;
}
