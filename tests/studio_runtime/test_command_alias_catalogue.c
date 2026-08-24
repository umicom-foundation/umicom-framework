/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_command_alias_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime command alias catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/command_alias_catalogue.h"
int main(void)
{
    size_t i,j;
    assert(umi_studio_command_alias_count()==22U);
    assert(umi_studio_command_alias_find("view.explorer")!=NULL);
    for(i=0U;i<umi_studio_command_alias_count();++i)
        for(j=i+1U;j<umi_studio_command_alias_count();++j)
            assert(strcmp(umi_studio_command_alias_at(i)->alias_id,
                          umi_studio_command_alias_at(j)->alias_id)!=0);
    return 0;
}

