/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/prompt.c
 *
 * PURPOSE:
 *   Format the enabled/allowed tool surface for a provider without exposing
 *   service pointers or product-specific implementation details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/prompt.h"

#include <stdio.h>
#include <string.h>

/* Provide the append text operation used by this module and its client applications. */
static UmiStatus append_text(
    char *out_text,
    size_t capacity,
    size_t *used,
    const char *text)
{
    const size_t length = strlen(text);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*used + length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(out_text + *used, text, length);
    *used += length;
    out_text[*used] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding tool call protocol operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_tool_call_protocol(void)
{
    return
        "To request one tool, emit exactly:\n"
        "UMICOM-TOOL-CALL/1\n"
        "TOOL|<tool-id>\n"
        "APPROVED|0\n"
        "ARGUMENTS-BEGIN\n"
        "<one JSON object>\n"
        "ARGUMENTS-END\n"
        "Do not claim approval yourself. APPROVED must be 0 unless the host "
        "explicitly returns an approved call.";
}

/*
 * Provide the ai coding tool plan protocol operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_tool_plan_protocol(void)
{
    return
        "For a multi-step plan, emit exactly:\n"
        "UMICOM-TOOL-PLAN/1\n"
        "PLAN|<plan-id>|<short title>\n"
        "RATIONALE-BEGIN\n<reason>\nRATIONALE-END\n"
        "STEP-BEGIN|<step-id>|<tool-id>|<required 0/1>|<continue 0/1>\n"
        "ARGUMENTS-BEGIN\n<one JSON object>\nARGUMENTS-END\n"
        "STEP-END\n"
        "Repeat STEP-BEGIN...STEP-END, then emit PLAN-END.";
}

/*
 * Provide the ai coding tool prompt build operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_prompt_build(
    const UmiAiCodingToolEnvironment *environment,
    char *out_text,
    size_t capacity)
{
    size_t used = 0U;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_text[0] = '\0';

    status = append_text(
        out_text,
        capacity,
        &used,
        "Available Umicom Framework developer tools:\n");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_ai_coding_tool_manifest_count(); ++index) {
        UmiAiCodingToolManifestEntry manifest;
        const UmiAiCodingToolDescriptor *descriptor;
        int approval_required = 0;
        char line[2048];

        status = umi_ai_coding_tool_manifest_at(index, &manifest);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        descriptor = umi_ai_coding_tool_catalogue_find(manifest.tool_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (descriptor == NULL) continue;

        status = umi_ai_coding_tool_policy_check(
            &environment->policy,
            descriptor,
            0,
            &approval_required);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK && !approval_required) {
            continue;
        }

        {
            int written = snprintf(
                line,
                sizeof(line),
                "- %s | %s | args=%s | risk=%s | approval=%s\n",
                descriptor->tool_id,
                descriptor->description,
                manifest.arguments_schema,
                umi_ai_coding_tool_risk_text(descriptor->risk),
                approval_required ? "required" : "not-required");

            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (written < 0 || (size_t)written >= sizeof(line)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }

        status = append_text(out_text, capacity, &used, line);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    status = append_text(out_text, capacity, &used, "\n");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = append_text(
            out_text,
            capacity,
            &used,
            umi_ai_coding_tool_call_protocol());
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = append_text(out_text, capacity, &used, "\n\n");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = append_text(
            out_text,
            capacity,
            &used,
            umi_ai_coding_tool_plan_protocol());
    }

    return status;
}
