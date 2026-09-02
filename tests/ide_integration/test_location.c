/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_location.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration location.
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
#include "umicom/ide_integration/location.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiIdeLocation location;
    umi_ide_location_init(&location);
    assert(!umi_ide_location_has_target(&location));
    assert(umi_ide_location_set_path(
        &location, "src/main.c", 10U, 2U) == UMI_STATUS_OK);
    assert(umi_ide_location_has_target(&location));
    assert(strcmp(location.path, "src/main.c") == 0);
    assert(location.line == 10U);
    return 0;
}

