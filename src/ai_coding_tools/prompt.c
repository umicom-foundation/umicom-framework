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

static UmiStatus append_text(
    char *out_text,
    size_t capacity,
    size_t *used,
    const char *text)
{
    const size_t length = strlen(text);

    if (*used + length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(out_text + *used, text, length);
    *used += length;
    out_text[*used] = '\0';
    return UMI_STATUS_OK;
}

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

UmiStatus umi_ai_coding_tool_prompt_build(
    const UmiAiCodingToolEnvironment *environment,
    char *out_text,
    size_t capacity)
{
    size_t used = 0U;
    size_t index;
    UmiStatus status;

    if (environment == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_text[0] = '\0';

    status = append_text(
        out_text,
        capacity,
        &used,
        "Available Umicom Framework developer tools:\n");
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < umi_ai_coding_tool_manifest_count(); ++index) {
        UmiAiCodingToolManifestEntry manifest;
        const UmiAiCodingToolDescriptor *descriptor;
        int approval_required = 0;
        char line[2048];

        status = umi_ai_coding_tool_manifest_at(index, &manifest);
        if (status != UMI_STATUS_OK) return status;

        descriptor = umi_ai_coding_tool_catalogue_find(manifest.tool_id);
        if (descriptor == NULL) continue;

        status = umi_ai_coding_tool_policy_check(
            &environment->policy,
            descriptor,
            0,
            &approval_required);

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

            if (written < 0 || (size_t)written >= sizeof(line)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }

        status = append_text(out_text, capacity, &used, line);
        if (status != UMI_STATUS_OK) return status;
    }

    status = append_text(out_text, capacity, &used, "\n");
    if (status == UMI_STATUS_OK) {
        status = append_text(
            out_text,
            capacity,
            &used,
            umi_ai_coding_tool_call_protocol());
    }
    if (status == UMI_STATUS_OK) {
        status = append_text(out_text, capacity, &used, "\n\n");
    }
    if (status == UMI_STATUS_OK) {
        status = append_text(
            out_text,
            capacity,
            &used,
            umi_ai_coding_tool_plan_protocol());
    }

    return status;
}
