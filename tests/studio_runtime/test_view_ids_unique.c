/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_view_ids_unique.c
 *
 * PURPOSE:
 *   Behavioral regression coverage for Studio runtime view ids unique.
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
    const char *ids[]={
        umi_studio_view_id_runtime_overview(),
        umi_studio_view_id_document_tabs(),
        umi_studio_view_id_status(),
        umi_studio_view_id_command_search(),
        umi_studio_view_id_layout_browser(),
        umi_studio_view_id_close_guard(),
        umi_studio_view_id_contract(),
        umi_studio_view_id_selection(),
        umi_studio_view_id_shell_state()
    };
    size_t i,j;
    for(i=0U;i<sizeof(ids)/sizeof(ids[0]);++i)
        for(j=i+1U;j<sizeof(ids)/sizeof(ids[0]);++j)
            assert(strcmp(ids[i],ids[j])!=0);
    return 0;
}

