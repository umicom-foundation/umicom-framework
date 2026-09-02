/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_learning_objective.c
 *
 * PURPOSE:
 *   Implement the test learning objective behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_objective.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTeacherLearningObjective value;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_teacher_learning_objective_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_teacher_learning_objective_validate(&value) != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_teacher_learning_objective_priority(&value, 60U) != 70U) return 3;
    return 0;
}
