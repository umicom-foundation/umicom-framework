/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/tools_catalogue.c
 *
 * PURPOSE:
 *   Return the richer Framework coding-tool catalogue for transparent agent/UI
 *   discovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/tools_catalogue.h"
#include "umicom/ai_coding_tools/catalogue.h"
#include "../tool_support.h"

/*
 * Provide the ai coding tool tools catalogue descriptor operation used by this module and
 * its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_tools_catalogue_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "tools.catalogue",
        .label = "List AI Coding Tools",
        .description = "List coding tools with risk and approval metadata.",
        .permission = "tools.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = 0U,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

/*
 * Provide the ai coding tool tools catalogue invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_tools_catalogue_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiLanguageRuntimeJsonWriter writer;
    size_t index;
    UmiStatus status;

    (void)arguments_json;
    (void)user_data;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"tools\":[");

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_ai_coding_tool_catalogue_count(); ++index) {
        const UmiAiCodingToolDescriptor *descriptor =
            umi_ai_coding_tool_catalogue_at(index);

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (descriptor == NULL) continue;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index > 0U) {
            (void)umi_language_runtime_json_writer_raw(&writer, ",");
        }

        (void)umi_language_runtime_json_writer_raw(&writer, "{\"id\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, descriptor->tool_id);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"label\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, descriptor->label);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"risk\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, umi_ai_coding_tool_risk_text(descriptor->risk));
        (void)umi_language_runtime_json_writer_raw(
            &writer, ",\"requiresApproval\":");
        (void)umi_language_runtime_json_writer_bool(
            &writer, descriptor->requires_approval);
        (void)umi_language_runtime_json_writer_raw(&writer, "}");

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (writer.status != UMI_STATUS_OK) return writer.status;
    }

    (void)umi_language_runtime_json_writer_raw(&writer, "],\"count\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, umi_ai_coding_tool_catalogue_count());
    return umi_ai_coding_tool_write_status_end(&writer);
}
