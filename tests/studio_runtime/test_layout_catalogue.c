/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_layout_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime layout catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/layout_catalogue.h"
int main(void)
{
    assert(umi_studio_layout_catalogue_count()==7U);
    assert(umi_studio_layout_catalogue_find(
        "umicom.studio.layout.default")!=NULL);
    assert(strcmp(umi_studio_layout_catalogue_at(0)->title,
                  "Default Development")==0);
    return 0;
}

