/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/workspace_list.c
 *
 * PURPOSE:
 *   Enumerate repository files through the existing bounded scanner and return a
 *   JSON array suitable for model reasoning without exposing OS directory APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/workspace_list.h"
#include "../tool_support.h"

#include <string.h>

typedef struct ListContext {
    UmiLanguageRuntimeJsonWriter *writer;
    const char *contains;
    uint64_t limit;
    uint64_t count;
} ListContext;

/* Provide the visit file operation used by this module and its client applications. */
static UmiStatus visit_file(
    void *user_data,
    const UmiAiCodingScanEntry *entry,
    int *out_descend)
{
    ListContext *context = (ListContext *)user_data;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || entry == NULL || out_descend == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_descend = 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (entry->directory) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (context->count >= context->limit) return UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context->contains != NULL &&
        context->contains[0] != '\0' &&
        strstr(entry->relative_path, context->contains) == NULL) {
        return UMI_STATUS_OK;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (context->count > 0U) {
        (void)umi_language_runtime_json_writer_raw(context->writer, ",");
    }

    (void)umi_language_runtime_json_writer_raw(
        context->writer, "{\"path\":");
    (void)umi_language_runtime_json_writer_string(
        context->writer, entry->relative_path);
    (void)umi_language_runtime_json_writer_raw(
        context->writer, ",\"bytes\":");
    (void)umi_language_runtime_json_writer_uint64(
        context->writer, entry->byte_size);
    (void)umi_language_runtime_json_writer_raw(context->writer, "}");

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (context->writer->status != UMI_STATUS_OK) {
        return context->writer->status;
    }

    context->count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding tool workspace list descriptor operation used by this module and
 * its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_workspace_list_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "workspace.list",
        .label = "List Workspace Files",
        .description = "List repository files with optional substring filtering.",
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
 * Provide the ai coding tool workspace list invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_workspace_list_invoke(
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
    ListContext context;
    char contains[256];
    uint64_t limit = 100U;
    size_t scanned = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_json_optional_string(
        &document, "contains", "", contains, sizeof(contains));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_uint64(
            &document, "limit", 100U, &limit);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (limit == 0U || limit > UMI_AI_CODING_TOOL_MAX_FILE_RESULTS) {
        limit = UMI_AI_CODING_TOOL_MAX_FILE_RESULTS;
    }

    umi_ai_coding_ignore_policy_init(&ignore);

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"files\":[");

    (void)memset(&context, 0, sizeof(context));
    context.writer = &writer;
    context.contains = contains;
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
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"scanned\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, scanned);
    return umi_ai_coding_tool_write_status_end(&writer);
}
