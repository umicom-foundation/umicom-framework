/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_teacher_prompt.c
 *
 * PURPOSE:
 *   Implement the test teacher prompt behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/teacher_prompt.h"
int main(void) {
    UmiTeacherTeacherPrompt value;
    if (umi_teacher_teacher_prompt_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    if (umi_teacher_teacher_prompt_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_teacher_teacher_prompt_priority(&value, 60U) != 70U) return 3;
    return 0;
}
