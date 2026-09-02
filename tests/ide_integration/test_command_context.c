/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_command_context.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration command context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ide_integration/command_context.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiIdeCommandContext context;
    umi_ide_command_context_init(&context);
    assert(!context.has_editor_selection);
    assert(context.problem_index == 0U);
    return 0;
}

