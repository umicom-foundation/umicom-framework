/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/language_diagnostics.c
 *
 * PURPOSE:
 *   Return bounded LSP/native language diagnostics with optional document filter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/language_diagnostics.h"
#include "../tool_support.h"

#include <string.h>

/*
 * Provide the ai coding tool language diagnostics descriptor operation used by this module
 * and its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_language_diagnostics_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "language.diagnostics",
        .label = "Language Diagnostics",
        .description = "List Framework language diagnostics.",
        .permission = "diagnostics.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_DIAGNOSTIC_READ,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

/*
 * Provide the ai coding tool language diagnostics invoke operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_tool_language_diagnostics_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeJsonWriter writer;
    UmiLanguageDiagnosticRegistry *registry;
    char document_id[128];
    uint64_t limit = 100U;
    uint64_t emitted = 0U;
    size_t index;
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
        status = umi_ai_coding_tool_json_optional_string(
            &document, "documentId", "", document_id, sizeof(document_id));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_uint64(
            &document, "limit", 100U, &limit);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (limit == 0U || limit > 256U) limit = 256U;

    registry = umi_language_service_diagnostic(environment->language);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"diagnostics\":[");

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_language_diagnostic_registry_count(registry) &&
         emitted < limit;
         ++index) {
        UmiLanguageDiagnosticSnapshot diagnostic;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_language_diagnostic_registry_at(
                registry, index, &diagnostic) != UMI_STATUS_OK) {
            continue;
        }

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (document_id[0] != '\0' &&
            strcmp(diagnostic.document_id, document_id) != 0) {
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (emitted > 0U) {
            (void)umi_language_runtime_json_writer_raw(&writer, ",");
        }

        (void)umi_language_runtime_json_writer_raw(&writer, "{\"id\":");
        (void)umi_language_runtime_json_writer_string(&writer, diagnostic.id);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"documentId\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, diagnostic.document_id);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"severity\":");
        (void)umi_language_runtime_json_writer_uint64(
            &writer, (uint64_t)diagnostic.severity);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"line\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, diagnostic.line);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"column\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, diagnostic.column);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"source\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, diagnostic.source);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"code\":");
        (void)umi_language_runtime_json_writer_string(&writer, diagnostic.code);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"message\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, diagnostic.message);
        (void)umi_language_runtime_json_writer_raw(&writer, "}");

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (writer.status != UMI_STATUS_OK) return writer.status;
        emitted += 1U;
    }

    (void)umi_language_runtime_json_writer_raw(&writer, "],\"count\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, emitted);
    return umi_ai_coding_tool_write_status_end(&writer);
}
