/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/ai.c
 *
 * PURPOSE:
 *   Define Framework-owned Ai commands. Native lifecycle
 *   actions are executable directly; product/UI actions are enabled only after
 *   an application binds a handler through the action registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/commands/ai.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "ai.assistant.open",
        "AI Coding Assistant",
        "AI",
        "Open the repository-aware AI assistant.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "ai.chat.new",
        "New AI Chat",
        "AI",
        "Start a new AI conversation.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "ai.explain-selection",
        "Explain Selection",
        "AI",
        "Explain the selected code using governed context.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "ai.fix-problems",
        "Fix Problems",
        "AI",
        "Plan fixes for selected diagnostics.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE
    },
    {
        "ai.agent.open",
        "Vibe Coding Agent",
        "AI",
        "Open agentic plan/edit/build/test workflow.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_ai_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_ai_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
