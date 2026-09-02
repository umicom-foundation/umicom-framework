/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/language_symbol_find.c
 *
 * PURPOSE:
 *   Resolve one stable language symbol identifier through the existing registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/language_symbol_find.h"
#include "../tool_support.h"

/*
 * Provide the ai coding tool language symbol find descriptor operation used by this module
 * and its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_language_symbol_find_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "language.symbol-find",
        .label = "Find Language Symbol",
        .description = "Find one Framework language symbol by stable ID.",
        .permission = "language.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_LANGUAGE_READ,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

/*
 * Provide the ai coding tool language symbol find invoke operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_tool_language_symbol_find_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeJsonWriter writer;
    UmiLanguageSymbolSnapshot symbol;
    UmiLanguageSymbolRegistry *registry;
    char id[128];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || environment->language == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_required_string(
            &document, "id", id, sizeof(id));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    registry = umi_language_service_symbol(environment->language);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_language_symbol_registry_find(registry, id, &symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"symbol\":{\"id\":");
    (void)umi_language_runtime_json_writer_string(&writer, symbol.id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"documentId\":");
    (void)umi_language_runtime_json_writer_string(&writer, symbol.document_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"name\":");
    (void)umi_language_runtime_json_writer_string(&writer, symbol.name);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"kind\":");
    (void)umi_language_runtime_json_writer_string(&writer, symbol.kind);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"container\":");
    (void)umi_language_runtime_json_writer_string(&writer, symbol.container);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"line\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, symbol.line);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"column\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, symbol.column);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    return umi_ai_coding_tool_write_status_end(&writer);
}
