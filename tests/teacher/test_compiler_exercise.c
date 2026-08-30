/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_compiler_exercise.c
 *
 * PURPOSE:
 *   Implement the test compiler exercise behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/compiler_exercise.h"
int main(void) {
    UmiTeacherCompilerExercise value;
    if (umi_teacher_compiler_exercise_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    if (umi_teacher_compiler_exercise_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_teacher_compiler_exercise_priority(&value, 60U) != 70U) return 3;
    return 0;
}
