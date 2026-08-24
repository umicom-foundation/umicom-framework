/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_command_context.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration command context.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ide_integration/command_context.h"
int main(void)
{
    UmiIdeCommandContext context;
    umi_ide_command_context_init(&context);
    assert(!context.has_editor_selection);
    assert(context.problem_index == 0U);
    return 0;
}

