/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_command_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration command catalogue.
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
#include "umicom/ide_integration/command.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    size_t i,j;
    assert(umi_ide_command_count() == 29U);
    assert(umi_ide_command_find("ide.ai.fix-problem") != NULL);
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<umi_ide_command_count();++i) {
        const UmiIdeCommandDescriptor *a=umi_ide_command_at(i);
        assert(a!=NULL);
        /* Visit each bounded item once so every record receives the same rule. */
        for(j=i+1U;j<umi_ide_command_count();++j)
            assert(strcmp(a->command_id,umi_ide_command_at(j)->command_id)!=0);
    }
    return 0;
}

