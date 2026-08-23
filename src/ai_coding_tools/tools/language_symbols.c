/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/language_symbols.c
 *
 * PURPOSE:
 *   Query the existing provider-neutral language symbol registry with optional
 *   name/document filtering. LSP/native providers remain authoritative.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/language_symbols.h"
#include "../tool_support.h"

#include <string.h>

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_language_symbols_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "language.symbols",
        .label = "List Language Symbols",
        .description = "Query Framework language symbols by name/document.",
        .permission = "language.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_LANGUAGE_READ,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_language_symbols_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeJsonWriter writer;
    UmiLanguageSymbolRegistry *registry;
    char query[256];
    char document_id[128];
    uint64_t limit = 100U;
    size_t index;
    uint64_t emitted = 0U;
    UmiStatus status;

    if (environment == NULL || environment->language == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_string(
            &document, "query", "", query, sizeof(query));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_string(
            &document, "documentId", "", document_id, sizeof(document_id));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_uint64(
            &document, "limit", 100U, &limit);
    }
    if (status != UMI_STATUS_OK) return status;

    if (limit == 0U || limit > 256U) limit = 256U;

    registry = umi_language_service_symbol(environment->language);
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"symbols\":[");

    for (index = 0U;
         index < umi_language_symbol_registry_count(registry) &&
         emitted < limit;
         ++index) {
        UmiLanguageSymbolSnapshot symbol;

        if (umi_language_symbol_registry_at(
                registry, index, &symbol) != UMI_STATUS_OK) {
            continue;
        }

        if (query[0] != '\0' &&
            strstr(symbol.name, query) == NULL) {
            continue;
        }

        if (document_id[0] != '\0' &&
            strcmp(symbol.document_id, document_id) != 0) {
            continue;
        }

        if (emitted > 0U) {
            (void)umi_language_runtime_json_writer_raw(&writer, ",");
        }

        (void)umi_language_runtime_json_writer_raw(&writer, "{\"id\":");
        (void)umi_language_runtime_json_writer_string(&writer, symbol.id);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"documentId\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, symbol.document_id);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"name\":");
        (void)umi_language_runtime_json_writer_string(&writer, symbol.name);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"kind\":");
        (void)umi_language_runtime_json_writer_string(&writer, symbol.kind);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"container\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, symbol.container);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"line\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, symbol.line);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"column\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, symbol.column);
        (void)umi_language_runtime_json_writer_raw(&writer, "}");

        if (writer.status != UMI_STATUS_OK) return writer.status;
        emitted += 1U;
    }

    (void)umi_language_runtime_json_writer_raw(&writer, "],\"count\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, emitted);
    return umi_ai_coding_tool_write_status_end(&writer);
}
