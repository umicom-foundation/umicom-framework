/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_types.c
 *
 * PURPOSE:
 *   Implement the test types behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>
#include "umicom/teacher/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_teacher_language_text(UMI_TEACHER_LANGUAGE_C23), "c23") != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_teacher_percent(3U, 4U) != 75U) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_teacher_clamp_score(140U) != 100U) return 3;
    return 0;
}
