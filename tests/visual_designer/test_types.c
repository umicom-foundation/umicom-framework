/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_types.c
 *
 * PURPOSE:
 *   Validate define stable toolkit-neutral visual designer identifiers, limits and shared enums.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/types.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    char value[16];
    CHECK(umi_rad_copy_text(value, sizeof value, "canvas") == UMI_STATUS_OK);
    CHECK(strcmp(value, "canvas") == 0);
    CHECK(umi_rad_id_valid("root"));
    CHECK(!umi_rad_id_valid(""));
    return 0;
}
