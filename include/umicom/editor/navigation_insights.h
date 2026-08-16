/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/navigation_insights.h
 *
 * PURPOSE:
 *   Coordinate Framework-owned peek, references, call hierarchy and type
 *   hierarchy models behind one toolkit-neutral source insight service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_NAVIGATION_INSIGHTS_H
#define UMICOM_EDITOR_NAVIGATION_INSIGHTS_H

#include "umicom/editor/call_hierarchy.h"
#include "umicom/editor/peek_definition.h"
#include "umicom/editor/reference_results.h"
#include "umicom/editor/type_hierarchy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_NAVIGATION_INSIGHTS_API_VERSION 1U

typedef enum UmiEditorNavigationInsightSurface {
    UMI_EDITOR_NAVIGATION_INSIGHT_NONE = 0,
    UMI_EDITOR_NAVIGATION_INSIGHT_PEEK_DEFINITION = 1,
    UMI_EDITOR_NAVIGATION_INSIGHT_REFERENCES = 2,
    UMI_EDITOR_NAVIGATION_INSIGHT_CALL_HIERARCHY = 3,
    UMI_EDITOR_NAVIGATION_INSIGHT_TYPE_HIERARCHY = 4
} UmiEditorNavigationInsightSurface;

typedef struct UmiEditorNavigationInsightsSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationInsightSurface active_surface;
    size_t peek_result_count;
    size_t reference_result_count;
    size_t call_hierarchy_node_count;
    size_t type_hierarchy_node_count;
    uint64_t revision;
} UmiEditorNavigationInsightsSnapshot;

typedef struct UmiEditorNavigationInsights UmiEditorNavigationInsights;

UmiStatus umi_editor_navigation_insights_create(
    UmiEditorNavigationInsights **out_insights);
void umi_editor_navigation_insights_destroy(
    UmiEditorNavigationInsights *insights);
UmiStatus umi_editor_navigation_insights_open_peek(
    UmiEditorNavigationInsights *insights,
    const UmiEditorSourceLocation *anchor,
    const UmiEditorNavigationResultSet *results);
UmiStatus umi_editor_navigation_insights_open_references(
    UmiEditorNavigationInsights *insights,
    const UmiEditorNavigationResultSet *results);
UmiStatus umi_editor_navigation_insights_open_call_hierarchy(
    UmiEditorNavigationInsights *insights,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorSymbolRelationshipGraph *relationship_graph,
    const char *root_symbol_id,
    UmiEditorCallHierarchyDirection direction,
    const UmiEditorSymbolHierarchyBuildOptions *options);
UmiStatus umi_editor_navigation_insights_open_type_hierarchy(
    UmiEditorNavigationInsights *insights,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorSymbolRelationshipGraph *relationship_graph,
    const char *root_symbol_id,
    UmiEditorTypeHierarchyDirection direction,
    const UmiEditorSymbolHierarchyBuildOptions *options);
UmiStatus umi_editor_navigation_insights_close(
    UmiEditorNavigationInsights *insights);
UmiEditorPeekDefinitionModel *umi_editor_navigation_insights_peek(
    UmiEditorNavigationInsights *insights);
UmiEditorReferenceResults *umi_editor_navigation_insights_references(
    UmiEditorNavigationInsights *insights);
UmiEditorSymbolHierarchy *umi_editor_navigation_insights_call_hierarchy(
    UmiEditorNavigationInsights *insights);
UmiEditorSymbolHierarchy *umi_editor_navigation_insights_type_hierarchy(
    UmiEditorNavigationInsights *insights);
UmiStatus umi_editor_navigation_insights_snapshot(
    const UmiEditorNavigationInsights *insights,
    UmiEditorNavigationInsightsSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_INSIGHTS_H */
