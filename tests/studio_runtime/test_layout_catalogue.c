/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_layout_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime layout catalogue.
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
#include "umicom/studio_runtime/layout_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(umi_studio_layout_catalogue_count()==7U);
    assert(umi_studio_layout_catalogue_find(
        "umicom.studio.layout.default")!=NULL);
    assert(strcmp(umi_studio_layout_catalogue_at(0)->title,
                  "Default Development")==0);
    return 0;
}

