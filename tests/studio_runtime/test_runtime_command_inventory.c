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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    size_t i,j;
    int has_code_preview = 0;
    /* The inventory includes the dedicated code-and-live-preview layout. */
    assert(umi_studio_runtime_command_count()==17U);
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<umi_studio_runtime_command_count();++i) {
        const char *a=umi_studio_runtime_command_id_at(i);
        assert(a!=NULL);
        /* Remember the new workflow while still checking every identifier. */
        if (strcmp(a, "studio.layout.code-preview") == 0) {
            has_code_preview = 1;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for(j=i+1U;j<umi_studio_runtime_command_count();++j)
            assert(strcmp(a,umi_studio_runtime_command_id_at(j))!=0);
    }
    assert(has_code_preview);
    return 0;
}
