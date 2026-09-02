/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/ai_action_bridge.c
 *
 * PURPOSE:
 *   Implement editor/Problems -> operational coding-agent routing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/ai_action_bridge.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the ide ai run selection operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_ai_run_selection(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeEditorSelection *selection,
    UmiAiCodingTaskKind task_kind,
    const char *task_id,
    const char *request_id,
    const char *session_id,
    const char *instruction)
{
    UmiIdeIntegrationBindings *bindings;
    UmiAiCodingRuntimePlatform *coding;
    UmiAiCodingRequest request;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || selection == NULL ||
        task_id == NULL || task_id[0] == '\0' ||
        request_id == NULL || request_id[0] == '\0' ||
        session_id == NULL || session_id[0] == '\0' ||
        instruction == NULL || instruction[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bindings = umi_ide_integration_platform_bindings(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || bindings->ai_developer == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    coding = umi_ai_developer_experience_platform_coding(
        bindings->ai_developer);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coding == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ide_ai_request_from_selection(
        selection,
        task_kind,
        request_id,
        session_id,
        umi_ide_integration_platform_workspace_root(platform),
        instruction,
        &request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_runtime_platform_run(
        coding,
        task_id,
        &request);

    (void)umi_ai_developer_experience_platform_refresh(
        bindings->ai_developer);

    return status;
}

/*
 * Provide the ide ai fix problem operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_ai_fix_problem(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeEditorSelection *selection,
    size_t problem_index,
    const char *task_id,
    const char *request_id,
    const char *session_id)
{
    UmiIdeIntegrationBindings *bindings;
    char problem[UMI_IDE_INTEGRATION_TEXT_CAPACITY];
    char instruction[UMI_AI_TEXT_CAPACITY];
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bindings = umi_ide_integration_platform_bindings(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || bindings->problems == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ide_problem_ai_summary(
        bindings->problems,
        problem_index,
        problem,
        sizeof(problem));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    written = snprintf(
        instruction,
        sizeof(instruction),
        "Fix the selected problem while preserving existing functionality, "
        "comments and unrelated code. Return a governed complete-file patch. "
        "Problem: %.1500s",
        problem);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(instruction)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return umi_ide_ai_run_selection(
        platform,
        selection,
        UMI_AI_CODING_TASK_REFACTOR,
        task_id,
        request_id,
        session_id,
        instruction);
}
