/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_perspective.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration perspective.
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
#include "umicom/ide_integration/perspective.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperWorkbenchPerspectiveDefinition *p =
        umi_ide_integrated_development_perspective();
    assert(p != NULL);
    assert(strcmp(p->perspective_id,
                  "umicom.perspective.integrated-development") == 0);
    assert(p->surface_count == 16U);
    assert(umi_developer_workbench_perspective_validate(p) == UMI_STATUS_OK);
    return 0;
}

