/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_view_ids.c
 *
 * PURPOSE:
 *   Integration regression coverage for view ids.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/view_ids.h"
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

