/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_mastery_evidence.c
 *
 * PURPOSE:
 *   Implement the test mastery evidence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/mastery_evidence.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTeacherMasteryEvidence value;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_teacher_mastery_evidence_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_teacher_mastery_evidence_validate(&value) != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_teacher_mastery_evidence_priority(&value, 60U) != 70U) return 3;
    return 0;
}
