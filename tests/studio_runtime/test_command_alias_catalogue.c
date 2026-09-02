/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_command_alias_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime command alias catalogue.
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
#include "umicom/studio_runtime/command_alias_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    size_t i,j;
    assert(umi_studio_command_alias_count()==23U);
    assert(umi_studio_command_alias_find("view.explorer")!=NULL);
    assert(umi_studio_command_alias_find("designer.open")!=NULL);
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<umi_studio_command_alias_count();++i)
        /* Visit each bounded item once so every record receives the same rule. */
        for(j=i+1U;j<umi_studio_command_alias_count();++j)
            assert(strcmp(umi_studio_command_alias_at(i)->alias_id,
                          umi_studio_command_alias_at(j)->alias_id)!=0);
    return 0;
}
