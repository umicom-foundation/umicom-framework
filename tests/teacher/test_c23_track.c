/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_c23_track.c
 *
 * PURPOSE:
 *   Implement the test c23 track behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/c23_track.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTeacherC23Track value;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_teacher_c23_track_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_teacher_c23_track_validate(&value) != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_teacher_c23_track_priority(&value, 60U) != 70U) return 3;
    return 0;
}
