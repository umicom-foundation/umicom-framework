/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/workspace_search.c
 *
 * PURPOSE:
 *   Search bounded workspace text files for a literal query. The implementation
 *   never shells out to grep/ripgrep, so query text cannot become a command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/workspace_search.h"
#include "../tool_support.h"
#include "umicom/ai_coding_runtime/language.h"

#include <string.h>

typedef struct SearchContext {
    UmiAiCodingToolEnvironment *environment;
    UmiLanguageRuntimeJsonWriter *writer;
    const char *query;
    uint64_t limit;
    uint64_t count;
} SearchContext;

/* Provide the emit matches operation used by this module and its client applications. */
static UmiStatus emit_matches(
    SearchContext *context,
    const char *path,
    const char *text)
{
    const char *cursor = text;
    uint32_t line = 1U;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0' && context->count < context->limit) {
        const char *line_end = strchr(cursor, '\n');
        const size_t length =
            line_end != NULL ? (size_t)(line_end - cursor) : strlen(cursor);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length > 0U) {
            char line_text[1024];
            const size_t copy_length =
                length < sizeof(line_text) - 1U
                    ? length
                    : sizeof(line_text) - 1U;

            (void)memcpy(line_text, cursor, copy_length);
            line_text[copy_length] = '\0';

            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (strstr(line_text, context->query) != NULL) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (context->count > 0U) {
                    (void)umi_language_runtime_json_writer_raw(
                        context->writer, ",");
                }

                (void)umi_language_runtime_json_writer_raw(
                    context->writer, "{\"path\":");
                (void)umi_language_runtime_json_writer_string(
                    context->writer, path);
                (void)umi_language_runtime_json_writer_raw(
                    context->writer, ",\"line\":");
                (void)umi_language_runtime_json_writer_uint64(
                    context->writer, line);
                (void)umi_language_runtime_json_writer_raw(
                    context->writer, ",\"text\":");
                (void)umi_language_runtime_json_writer_string(
                    context->writer, line_text);
                (void)umi_language_runtime_json_writer_raw(
                    context->writer, "}");

                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (context->writer->status != UMI_STATUS_OK) {
                    return context->writer->status;
                }

                context->count += 1U;
            }
        }

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (line_end == NULL) break;
        cursor = line_end + 1;
        line += 1U;
    }

    return UMI_STATUS_OK;
}

/* Provide the visit file operation used by this module and its client applications. */
static UmiStatus visit_file(
    void *user_data,
    const UmiAiCodingScanEntry *entry,
    int *out_descend)
{
    SearchContext *context = (SearchContext *)user_data;
    char text[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t length = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || entry == NULL || out_descend == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_descend = 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (entry->directory ||
        context->count >= context->limit ||
        !umi_ai_coding_runtime_path_is_text_source(entry->relative_path) ||
        entry->byte_size >= sizeof(text)) {
        return UMI_STATUS_OK;
    }

    status = context->environment->workspace.read(
        context->environment->workspace.user_data,
        entry->relative_path,
        text,
        sizeof(text),
        &length);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND ||
        status == UMI_STATUS_CAPACITY_EXCEEDED ||
        status == UMI_STATUS_PARSE_ERROR) {
        return UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)length;
    return emit_matches(context, entry->relative_path, text);
}

/*
 * Provide the ai coding tool workspace search descriptor operation used by this module and
 * its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_workspace_search_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "workspace.search",
        .label = "Search Workspace",
        .description = "Literal text search across repository source files.",
        .permission = "workspace.search",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_WORKSPACE_SEARCH,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

/*
 * Provide the ai coding tool workspace search invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_workspace_search_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeJsonWriter writer;
    UmiAiCodingIgnorePolicy ignore;
    SearchContext context;
    char query[512];
    uint64_t limit = 50U;
    size_t scanned = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_required_string(
            &document, "query", query, sizeof(query));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_uint64(
            &document, "limit", 50U, &limit);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (query[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (limit == 0U || limit > UMI_AI_CODING_TOOL_MAX_SEARCH_RESULTS) {
        limit = UMI_AI_CODING_TOOL_MAX_SEARCH_RESULTS;
    }

    umi_ai_coding_ignore_policy_init(&ignore);

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"matches\":[");

    (void)memset(&context, 0, sizeof(context));
    context.environment = environment;
    context.writer = &writer;
    context.query = query;
    context.limit = limit;

    status = umi_ai_coding_scan_workspace(
        environment->workspace_root,
        &ignore,
        visit_file,
        &context,
        &scanned);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, "],\"count\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, context.count);
    return umi_ai_coding_tool_write_status_end(&writer);
}
