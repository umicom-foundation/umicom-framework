/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/catalogue.c
 *
 * PURPOSE:
 *   Enumerate and register the complete built-in coding-tool surface into the
 *   existing UmiAiRuntime tool registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/catalogue.h"

#include <string.h>

#include "umicom/ai/tool.h"
#include "umicom/ai_coding_tools/tools/agent_apply.h"
#include "umicom/ai_coding_tools/tools/agent_approve.h"
#include "umicom/ai_coding_tools/tools/agent_cancel.h"
#include "umicom/ai_coding_tools/tools/agent_continue_repair.h"
#include "umicom/ai_coding_tools/tools/agent_reject.h"
#include "umicom/ai_coding_tools/tools/agent_snapshot.h"
#include "umicom/ai_coding_tools/tools/checkpoint_create.h"
#include "umicom/ai_coding_tools/tools/checkpoint_list.h"
#include "umicom/ai_coding_tools/tools/checkpoint_restore.h"
#include "umicom/ai_coding_tools/tools/debug_continue.h"
#include "umicom/ai_coding_tools/tools/debug_pause.h"
#include "umicom/ai_coding_tools/tools/debug_snapshot.h"
#include "umicom/ai_coding_tools/tools/debug_stack.h"
#include "umicom/ai_coding_tools/tools/debug_step_into.h"
#include "umicom/ai_coding_tools/tools/debug_step_out.h"
#include "umicom/ai_coding_tools/tools/debug_step_over.h"
#include "umicom/ai_coding_tools/tools/debug_threads.h"
#include "umicom/ai_coding_tools/tools/developer_build.h"
#include "umicom/ai_coding_tools/tools/developer_configure.h"
#include "umicom/ai_coding_tools/tools/developer_package.h"
#include "umicom/ai_coding_tools/tools/developer_run.h"
#include "umicom/ai_coding_tools/tools/developer_test.h"
#include "umicom/ai_coding_tools/tools/language_diagnostics.h"
#include "umicom/ai_coding_tools/tools/language_symbol_find.h"
#include "umicom/ai_coding_tools/tools/language_symbols.h"
#include "umicom/ai_coding_tools/tools/source_control_commit.h"
#include "umicom/ai_coding_tools/tools/source_control_diff.h"
#include "umicom/ai_coding_tools/tools/source_control_fetch.h"
#include "umicom/ai_coding_tools/tools/source_control_pull.h"
#include "umicom/ai_coding_tools/tools/source_control_push.h"
#include "umicom/ai_coding_tools/tools/source_control_stage.h"
#include "umicom/ai_coding_tools/tools/source_control_stage_all.h"
#include "umicom/ai_coding_tools/tools/source_control_status.h"
#include "umicom/ai_coding_tools/tools/source_control_unstage.h"
#include "umicom/ai_coding_tools/tools/tools_catalogue.h"
#include "umicom/ai_coding_tools/tools/tools_policy.h"
#include "umicom/ai_coding_tools/tools/workspace_exists.h"
#include "umicom/ai_coding_tools/tools/workspace_list.h"
#include "umicom/ai_coding_tools/tools/workspace_read.h"
#include "umicom/ai_coding_tools/tools/workspace_search.h"

typedef const UmiAiCodingToolDescriptor *(*DescriptorFactory)(void);
typedef UmiStatus (*InvokeFunction)(
    const char *,
    char *,
    size_t,
    void *);

static const DescriptorFactory DESCRIPTORS[] = {
    umi_ai_coding_tool_agent_apply_descriptor,
    umi_ai_coding_tool_agent_approve_descriptor,
    umi_ai_coding_tool_agent_cancel_descriptor,
    umi_ai_coding_tool_agent_continue_repair_descriptor,
    umi_ai_coding_tool_agent_reject_descriptor,
    umi_ai_coding_tool_agent_snapshot_descriptor,
    umi_ai_coding_tool_checkpoint_create_descriptor,
    umi_ai_coding_tool_checkpoint_list_descriptor,
    umi_ai_coding_tool_checkpoint_restore_descriptor,
    umi_ai_coding_tool_debug_continue_descriptor,
    umi_ai_coding_tool_debug_pause_descriptor,
    umi_ai_coding_tool_debug_snapshot_descriptor,
    umi_ai_coding_tool_debug_stack_descriptor,
    umi_ai_coding_tool_debug_step_into_descriptor,
    umi_ai_coding_tool_debug_step_out_descriptor,
    umi_ai_coding_tool_debug_step_over_descriptor,
    umi_ai_coding_tool_debug_threads_descriptor,
    umi_ai_coding_tool_developer_build_descriptor,
    umi_ai_coding_tool_developer_configure_descriptor,
    umi_ai_coding_tool_developer_package_descriptor,
    umi_ai_coding_tool_developer_run_descriptor,
    umi_ai_coding_tool_developer_test_descriptor,
    umi_ai_coding_tool_language_diagnostics_descriptor,
    umi_ai_coding_tool_language_symbol_find_descriptor,
    umi_ai_coding_tool_language_symbols_descriptor,
    umi_ai_coding_tool_source_control_commit_descriptor,
    umi_ai_coding_tool_source_control_diff_descriptor,
    umi_ai_coding_tool_source_control_fetch_descriptor,
    umi_ai_coding_tool_source_control_pull_descriptor,
    umi_ai_coding_tool_source_control_push_descriptor,
    umi_ai_coding_tool_source_control_stage_descriptor,
    umi_ai_coding_tool_source_control_stage_all_descriptor,
    umi_ai_coding_tool_source_control_status_descriptor,
    umi_ai_coding_tool_source_control_unstage_descriptor,
    umi_ai_coding_tool_tools_catalogue_descriptor,
    umi_ai_coding_tool_tools_policy_descriptor,
    umi_ai_coding_tool_workspace_exists_descriptor,
    umi_ai_coding_tool_workspace_list_descriptor,
    umi_ai_coding_tool_workspace_read_descriptor,
    umi_ai_coding_tool_workspace_search_descriptor
};

static const InvokeFunction INVOKERS[] = {
    umi_ai_coding_tool_agent_apply_invoke,
    umi_ai_coding_tool_agent_approve_invoke,
    umi_ai_coding_tool_agent_cancel_invoke,
    umi_ai_coding_tool_agent_continue_repair_invoke,
    umi_ai_coding_tool_agent_reject_invoke,
    umi_ai_coding_tool_agent_snapshot_invoke,
    umi_ai_coding_tool_checkpoint_create_invoke,
    umi_ai_coding_tool_checkpoint_list_invoke,
    umi_ai_coding_tool_checkpoint_restore_invoke,
    umi_ai_coding_tool_debug_continue_invoke,
    umi_ai_coding_tool_debug_pause_invoke,
    umi_ai_coding_tool_debug_snapshot_invoke,
    umi_ai_coding_tool_debug_stack_invoke,
    umi_ai_coding_tool_debug_step_into_invoke,
    umi_ai_coding_tool_debug_step_out_invoke,
    umi_ai_coding_tool_debug_step_over_invoke,
    umi_ai_coding_tool_debug_threads_invoke,
    umi_ai_coding_tool_developer_build_invoke,
    umi_ai_coding_tool_developer_configure_invoke,
    umi_ai_coding_tool_developer_package_invoke,
    umi_ai_coding_tool_developer_run_invoke,
    umi_ai_coding_tool_developer_test_invoke,
    umi_ai_coding_tool_language_diagnostics_invoke,
    umi_ai_coding_tool_language_symbol_find_invoke,
    umi_ai_coding_tool_language_symbols_invoke,
    umi_ai_coding_tool_source_control_commit_invoke,
    umi_ai_coding_tool_source_control_diff_invoke,
    umi_ai_coding_tool_source_control_fetch_invoke,
    umi_ai_coding_tool_source_control_pull_invoke,
    umi_ai_coding_tool_source_control_push_invoke,
    umi_ai_coding_tool_source_control_stage_invoke,
    umi_ai_coding_tool_source_control_stage_all_invoke,
    umi_ai_coding_tool_source_control_status_invoke,
    umi_ai_coding_tool_source_control_unstage_invoke,
    umi_ai_coding_tool_tools_catalogue_invoke,
    umi_ai_coding_tool_tools_policy_invoke,
    umi_ai_coding_tool_workspace_exists_invoke,
    umi_ai_coding_tool_workspace_list_invoke,
    umi_ai_coding_tool_workspace_read_invoke,
    umi_ai_coding_tool_workspace_search_invoke
};

size_t umi_ai_coding_tool_catalogue_count(void)
{
    return sizeof(DESCRIPTORS) / sizeof(DESCRIPTORS[0]);
}

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_catalogue_at(size_t index)
{
    return index < umi_ai_coding_tool_catalogue_count()
        ? DESCRIPTORS[index]()
        : NULL;
}

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_catalogue_find(
    const char *tool_id)
{
    size_t index;

    if (tool_id == NULL) return NULL;

    for (index = 0U; index < umi_ai_coding_tool_catalogue_count(); ++index) {
        const UmiAiCodingToolDescriptor *descriptor = DESCRIPTORS[index]();

        if (descriptor != NULL &&
            strcmp(descriptor->tool_id, tool_id) == 0) {
            return descriptor;
        }
    }

    return NULL;
}

UmiStatus umi_ai_coding_tool_register_all(
    UmiAiCodingToolEnvironment *environment)
{
    size_t index;

    if (environment == NULL || environment->ai_runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < umi_ai_coding_tool_catalogue_count(); ++index) {
        const UmiAiCodingToolDescriptor *descriptor = DESCRIPTORS[index]();
        UmiAiTool tool;
        UmiStatus status;

        if (descriptor == NULL || INVOKERS[index] == NULL) {
            return UMI_STATUS_INTERNAL_ERROR;
        }

        if (umi_ai_tool_registry_find(
                &environment->ai_runtime->tools,
                descriptor->tool_id) != NULL) {
            continue;
        }

        status = umi_ai_tool_init(
            &tool,
            descriptor->tool_id,
            descriptor->description,
            descriptor->permission,
            INVOKERS[index],
            environment);
        if (status != UMI_STATUS_OK) return status;

        tool.flags = (uint32_t)descriptor->risk;

        status = umi_ai_tool_registry_add(
            &environment->ai_runtime->tools,
            &tool);
        if (status != UMI_STATUS_OK) return status;
    }

    environment->revision += 1U;
    return UMI_STATUS_OK;
}
