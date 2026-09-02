/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_view_ids.c
 *
 * PURPOSE:
 *   Integration regression coverage for view ids.
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
#include "umicom/ide_integration/view_ids.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(strcmp(
        umi_ide_view_id_overview(),
        "umicom.ide.integration.overview") == 0);
    assert(strcmp(
        umi_ide_view_id_self_host(),
        "umicom.ide.integration.self-host") == 0);
    return 0;
}

