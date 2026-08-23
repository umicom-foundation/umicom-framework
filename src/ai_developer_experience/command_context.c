/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/command_context.c
 *
 * PURPOSE:
 *   Initialize AI developer command arguments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/command_context.h"

#include <string.h>

void umi_ai_developer_command_context_init(
    UmiAiDeveloperCommandContext *context)
{
    if (context != NULL) {
        (void)memset(context, 0, sizeof(*context));
    }
}
