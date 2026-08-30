/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/command_context.c
 *
 * PURPOSE:
 *   Initialize IDE command context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/command_context.h"

#include <string.h>

void umi_ide_command_context_init(UmiIdeCommandContext *context)
{
    if (context == NULL) return;
    (void)memset(context, 0, sizeof(*context));
    umi_ide_editor_selection_init(&context->editor_selection);
}
