/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_command_context.c
 *
 * PURPOSE:
 *   Contract coverage for AI Developer Experience command context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_developer_experience/command_context.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiDeveloperCommandContext context;
    umi_ai_developer_command_context_init(&context);
    assert(context.chat_session_id[0] == '\0');
    assert(context.file_index == 0U);
    return 0;
}

