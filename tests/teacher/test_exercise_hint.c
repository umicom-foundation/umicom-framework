/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_exercise_hint.c
 *
 * PURPOSE:
 *   Implement the test exercise hint behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/exercise_hint.h"
int main(void) {
    UmiTeacherExerciseHint value;
    if (umi_teacher_exercise_hint_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    if (umi_teacher_exercise_hint_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_teacher_exercise_hint_priority(&value, 60U) != 70U) return 3;
    return 0;
}
