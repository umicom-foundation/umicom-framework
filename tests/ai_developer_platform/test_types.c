/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_types.c
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
#include "umicom/ai/developer_platform/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_ai_dev_state_text(UMI_AI_DEV_STATE_READY), "ready") != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ai_dev_percent(25U, 100U) != 25U) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ai_dev_percent(120U, 100U) != 100U) return 3;
    return 0;
}
