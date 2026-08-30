/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_runtime_command_inventory.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime runtime command inventory.
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
#include "umicom/studio_runtime/runtime_commands.h"
int main(void)
{
    size_t i,j;
    assert(umi_studio_runtime_command_count()==14U);
    for(i=0U;i<umi_studio_runtime_command_count();++i) {
        const char *a=umi_studio_runtime_command_id_at(i);
        assert(a!=NULL);
        for(j=i+1U;j<umi_studio_runtime_command_count();++j)
            assert(strcmp(a,umi_studio_runtime_command_id_at(j))!=0);
    }
    return 0;
}

