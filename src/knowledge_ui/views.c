/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge_ui/views.c
 * PURPOSE: Build toolkit-neutral Knowledge Centre panes and source viewers.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * These functions expose ordinary view-model data.  GTK4, Qt6, Wt and web
 * frontends can render the same collections, citations and source ranges.
 */
#include "umicom/knowledge_ui/views.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "umicom/ui/command_view.h"

#define UMI_KNOWLEDGE_UI_VISIBLE_ROWS 24U

static UmiStatus set_string(UmiUiViewModel *view,
                            const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(
        &value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view,
                             const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_action(UmiUiViewModel *view,
                            size_t index,
                            const char *action_id,
                            const char *label,
                            const char *tooltip)
{
    UmiUiCommandViewAction action = {0};
    (void)snprintf(action.action_id, sizeof(action.action_id), "%s", action_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = 1;
    return umi_ui_command_view_set_action(view, index, &action);
}

static UmiStatus base_view(const char *view_id,
                           const char *kind,
                           const char *title,
                           const char *summary,
                           UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (view_id == NULL || out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    status = umi_ui_view_model_create(
        view_id, "umicom.knowledge-centre", UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "umicom.view-kind", kind);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "summary", summary);
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

UmiStatus umi_knowledge_ui_overview_view_create(
    const char *view_id, UmiKnowledgeService *service,
    UmiUiViewModel **out_view)
{
    UmiKnowledgeServiceSnapshot snapshot;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "knowledge-overview", "Knowledge Centre",
        "Offline-first document, project and code retrieval with citations.",
        out_view);
    if (status == UMI_STATUS_OK) {
        status = umi_knowledge_service_snapshot(service, &snapshot);
    }
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "knowledge.collections", (int64_t)snapshot.collection_count);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "knowledge.sources", (int64_t)snapshot.source_count);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "knowledge.chunks", (int64_t)snapshot.chunk_count);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "knowledge.embedding-dimension",
        (int64_t)snapshot.embedding_dimension);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "knowledge.provider", snapshot.default_provider_id);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "knowledge.last-source", snapshot.last_source_id);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "knowledge.last-query", snapshot.last_query);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "knowledge.last-results",
        (int64_t)snapshot.last_result_count);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.knowledge.search", "Search",
        "Search indexed project, code and document evidence");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.knowledge.ingest", "Add Source",
        "Ingest a source into the selected collection");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.knowledge.refresh", "Refresh",
        "Incrementally refresh changed sources");
    return status;
}

UmiStatus umi_knowledge_ui_collections_view_create(
    const char *view_id, UmiKnowledgeService *service,
    UmiUiViewModel **out_view)
{
    UmiKnowledgeServiceSnapshot snapshot;
    size_t count;
    size_t position;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "knowledge-collections", "Knowledge Collections",
        "Independent retrieval scopes for project, code and document corpora.",
        out_view);
    if (status == UMI_STATUS_OK) {
        status = umi_knowledge_service_snapshot(service, &snapshot);
    }
    count = status == UMI_STATUS_OK ? snapshot.collection_count : 0U;
    if (count > UMI_KNOWLEDGE_UI_VISIBLE_ROWS) count = UMI_KNOWLEDGE_UI_VISIBLE_ROWS;
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "knowledge-collections.row-count", (int64_t)count);
    for (position = 0U; status == UMI_STATUS_OK && position < count; ++position) {
        UmiKnowledgeCollection collection;
        char key[96];
        char text[512];
        status = umi_knowledge_service_collection_at(
            service, position, &collection);
        (void)snprintf(key, sizeof(key), "knowledge-collections.row.%zu", position);
        (void)snprintf(text, sizeof(text), "%s | %s | %s",
                       collection.collection_id, collection.display_name,
                       collection.enabled ? "enabled" : "disabled");
        if (status == UMI_STATUS_OK) status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.knowledge.collection-create",
        "New Collection", "Create an isolated knowledge collection");
    return status;
}

UmiStatus umi_knowledge_ui_sources_view_create(
    const char *view_id, UmiKnowledgeService *service,
    UmiUiViewModel **out_view)
{
    UmiKnowledgeCatalogue *catalogue;
    size_t count;
    size_t position;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "knowledge-sources", "Knowledge Sources",
        "Indexed sources with revision, classification and provenance.",
        out_view);
    catalogue = umi_knowledge_service_catalogue(service);
    count = umi_knowledge_catalogue_count(catalogue);
    if (count > UMI_KNOWLEDGE_UI_VISIBLE_ROWS) count = UMI_KNOWLEDGE_UI_VISIBLE_ROWS;
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "knowledge-sources.row-count", (int64_t)count);
    for (position = 0U; status == UMI_STATUS_OK && position < count; ++position) {
        UmiKnowledgeSource source;
        char key[96];
        char text[1024];
        status = umi_knowledge_catalogue_source_at(catalogue, position, &source);
        (void)snprintf(key, sizeof(key), "knowledge-sources.row.%zu", position);
        (void)snprintf(text, sizeof(text), "%s | %s | %s | revision %llu",
                       source.source_id, source.title,
                       umi_knowledge_source_kind_text(source.kind),
                       (unsigned long long)source.revision);
        if (status == UMI_STATUS_OK) status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.knowledge.refresh", "Refresh Sources",
        "Index only sources whose content identity changed");
    return status;
}

UmiStatus umi_knowledge_ui_results_view_create(
    const char *view_id, UmiKnowledgeService *service,
    const UmiKnowledgeMatch *matches, size_t match_count,
    UmiUiViewModel **out_view)
{
    size_t count = match_count;
    size_t position;
    UmiStatus status;
    if (service == NULL || (match_count > 0U && matches == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = base_view(
        view_id, "knowledge-results", "Knowledge Search Results",
        "Ranked evidence with exact source and line provenance.", out_view);
    if (count > UMI_KNOWLEDGE_UI_VISIBLE_ROWS) count = UMI_KNOWLEDGE_UI_VISIBLE_ROWS;
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "knowledge-results.row-count", (int64_t)count);
    for (position = 0U; status == UMI_STATUS_OK && position < count; ++position) {
        char key[96];
        char text[1024];
        (void)snprintf(key, sizeof(key), "knowledge-results.row.%zu", position);
        (void)snprintf(text, sizeof(text),
                       "%.4f | %.160s | lines %zu-%zu | %.700s",
                       matches[position].score,
                       matches[position].citation.title,
                       matches[position].citation.line_start,
                       matches[position].citation.line_end,
                       matches[position].chunk.text);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.knowledge.search", "Search Again",
        "Run another filtered knowledge query");
    return status;
}

UmiStatus umi_knowledge_ui_source_view_create(
    const char *view_id, const UmiKnowledgeMatch *match,
    UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (match == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "knowledge-source", "Knowledge Source Viewer",
        "The exact retrieved source range; generated text is not substituted.",
        out_view);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "knowledge-source.id", match->citation.source_id);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "knowledge-source.title", match->citation.title);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "knowledge-source.uri", match->citation.uri);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "knowledge-source.line-start",
        (int64_t)match->citation.line_start);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "knowledge-source.line-end",
        (int64_t)match->citation.line_end);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "knowledge-source.text", match->chunk.text);
    return status;
}
