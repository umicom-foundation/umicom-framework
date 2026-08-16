/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/navigation_insights.c
 *
 * PURPOSE:
 *   Coordinate reusable source insight models while leaving application shells
 *   responsible only for menus, view placement and visual rendering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/navigation_insights.h"

#include <stdlib.h>

struct UmiEditorNavigationInsights {
    UmiEditorPeekDefinitionModel *peek;
    UmiEditorReferenceResults *references;
    UmiEditorSymbolHierarchy *call_hierarchy;
    UmiEditorSymbolHierarchy *type_hierarchy;
    UmiEditorNavigationInsightSurface active_surface;
    uint64_t revision;
};

static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

UmiStatus umi_editor_navigation_insights_create(
    UmiEditorNavigationInsights **out_insights)
{
    UmiEditorNavigationInsights *insights;
    UmiStatus status;
    if (out_insights == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_insights = NULL;
    insights = (UmiEditorNavigationInsights *)calloc(1U, sizeof(*insights));
    if (insights == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_peek_definition_model_create(&insights->peek);
    if (status == UMI_STATUS_OK) {
        status = umi_editor_reference_results_create(&insights->references);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_symbol_hierarchy_create(&insights->call_hierarchy);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_symbol_hierarchy_create(&insights->type_hierarchy);
    }
    if (status != UMI_STATUS_OK) {
        umi_editor_navigation_insights_destroy(insights);
        return status;
    }
    insights->revision = 1U;
    *out_insights = insights;
    return UMI_STATUS_OK;
}

void umi_editor_navigation_insights_destroy(
    UmiEditorNavigationInsights *insights)
{
    if (insights == NULL) return;
    umi_editor_symbol_hierarchy_destroy(insights->type_hierarchy);
    umi_editor_symbol_hierarchy_destroy(insights->call_hierarchy);
    umi_editor_reference_results_destroy(insights->references);
    umi_editor_peek_definition_model_destroy(insights->peek);
    free(insights);
}

static UmiStatus activate(UmiEditorNavigationInsights *insights,
                          UmiEditorNavigationInsightSurface surface,
                          UmiStatus operation_status)
{
    if (operation_status == UMI_STATUS_OK) {
        insights->active_surface = surface;
        insights->revision = next_revision(insights->revision);
    }
    return operation_status;
}

UmiStatus umi_editor_navigation_insights_open_peek(
    UmiEditorNavigationInsights *insights,
    const UmiEditorSourceLocation *anchor,
    const UmiEditorNavigationResultSet *results)
{
    if (insights == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return activate(insights,
                    UMI_EDITOR_NAVIGATION_INSIGHT_PEEK_DEFINITION,
                    umi_editor_peek_definition_model_load(insights->peek,
                                                          anchor,
                                                          results));
}

UmiStatus umi_editor_navigation_insights_open_references(
    UmiEditorNavigationInsights *insights,
    const UmiEditorNavigationResultSet *results)
{
    if (insights == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return activate(insights,
                    UMI_EDITOR_NAVIGATION_INSIGHT_REFERENCES,
                    umi_editor_reference_results_load(insights->references,
                                                      results));
}

UmiStatus umi_editor_navigation_insights_open_call_hierarchy(
    UmiEditorNavigationInsights *insights,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorSymbolRelationshipGraph *relationship_graph,
    const char *root_symbol_id,
    UmiEditorCallHierarchyDirection direction,
    const UmiEditorSymbolHierarchyBuildOptions *options)
{
    if (insights == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return activate(insights,
                    UMI_EDITOR_NAVIGATION_INSIGHT_CALL_HIERARCHY,
                    umi_editor_call_hierarchy_build(
                        insights->call_hierarchy,
                        symbol_index,
                        relationship_graph,
                        root_symbol_id,
                        direction,
                        options));
}

UmiStatus umi_editor_navigation_insights_open_type_hierarchy(
    UmiEditorNavigationInsights *insights,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorSymbolRelationshipGraph *relationship_graph,
    const char *root_symbol_id,
    UmiEditorTypeHierarchyDirection direction,
    const UmiEditorSymbolHierarchyBuildOptions *options)
{
    if (insights == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return activate(insights,
                    UMI_EDITOR_NAVIGATION_INSIGHT_TYPE_HIERARCHY,
                    umi_editor_type_hierarchy_build(
                        insights->type_hierarchy,
                        symbol_index,
                        relationship_graph,
                        root_symbol_id,
                        direction,
                        options));
}

UmiStatus umi_editor_navigation_insights_close(
    UmiEditorNavigationInsights *insights)
{
    if (insights == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (insights->active_surface ==
        UMI_EDITOR_NAVIGATION_INSIGHT_PEEK_DEFINITION) {
        (void)umi_editor_peek_definition_model_hide(insights->peek);
    }
    insights->active_surface = UMI_EDITOR_NAVIGATION_INSIGHT_NONE;
    insights->revision = next_revision(insights->revision);
    return UMI_STATUS_OK;
}

UmiEditorPeekDefinitionModel *umi_editor_navigation_insights_peek(
    UmiEditorNavigationInsights *insights)
{
    return insights != NULL ? insights->peek : NULL;
}

UmiEditorReferenceResults *umi_editor_navigation_insights_references(
    UmiEditorNavigationInsights *insights)
{
    return insights != NULL ? insights->references : NULL;
}

UmiEditorSymbolHierarchy *umi_editor_navigation_insights_call_hierarchy(
    UmiEditorNavigationInsights *insights)
{
    return insights != NULL ? insights->call_hierarchy : NULL;
}

UmiEditorSymbolHierarchy *umi_editor_navigation_insights_type_hierarchy(
    UmiEditorNavigationInsights *insights)
{
    return insights != NULL ? insights->type_hierarchy : NULL;
}

UmiStatus umi_editor_navigation_insights_snapshot(
    const UmiEditorNavigationInsights *insights,
    UmiEditorNavigationInsightsSnapshot *out_snapshot)
{
    if (insights == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_NAVIGATION_INSIGHTS_API_VERSION;
    out_snapshot->active_surface = insights->active_surface;
    out_snapshot->peek_result_count =
        umi_editor_peek_definition_model_count(insights->peek);
    out_snapshot->reference_result_count =
        umi_editor_reference_results_count(insights->references);
    out_snapshot->call_hierarchy_node_count =
        umi_editor_symbol_hierarchy_count(insights->call_hierarchy);
    out_snapshot->type_hierarchy_node_count =
        umi_editor_symbol_hierarchy_count(insights->type_hierarchy);
    out_snapshot->revision = insights->revision;
    return UMI_STATUS_OK;
}
