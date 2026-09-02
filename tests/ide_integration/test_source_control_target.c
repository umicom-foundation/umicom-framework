/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_source_control_target.c
 *
 * PURPOSE:
 *   Integration regression coverage for source control target.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/source_control_bridge.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiIdeNavigationTarget target;
    assert(umi_ide_source_control_target(
        "src/main.c", 9U, &target) == UMI_STATUS_OK);
    assert(strcmp(target.location.path, "src/main.c") == 0);
    assert(target.location.line == 9U);
    return 0;
}

