/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_view_ids.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime view ids.
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
#include "umicom/studio_runtime/view_ids.h"
int main(void)
{
    assert(strcmp(umi_studio_view_id_runtime_overview(),
                  "umicom.studio.runtime.overview")==0);
    assert(strcmp(umi_studio_view_id_contract(),
                  "umicom.studio.runtime.contract")==0);
    return 0;
}

