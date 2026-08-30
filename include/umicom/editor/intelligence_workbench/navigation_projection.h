/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/navigation_projection.h
 *
 * PURPOSE:
 *   Project canonical peek, reference, call-hierarchy and type-hierarchy state
 *   into one stable, toolkit-neutral workbench model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_NAVIGATION_PROJECTION_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_NAVIGATION_PROJECTION_H

#include "umicom/editor/intelligence_workbench/call_hierarchy_view_model.h"
#include "umicom/editor/intelligence_workbench/navigation_target_set.h"
#include "umicom/editor/intelligence_workbench/projection.h"
#include "umicom/editor/intelligence_workbench/reference_results_model.h"
#include "umicom/editor/intelligence_workbench/type_hierarchy_view_model.h"
#include "umicom/editor/navigation_insights.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INTEL_NAVIGATION_PROJECTION_API_VERSION 1U

typedef struct UmiEditorIntelNavigationProjection {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorIntelNavigationTargetSet peek_targets;
    UmiEditorIntelReferenceResultsModel references;
    UmiEditorIntelCallHierarchyViewModel call_hierarchy;
    UmiEditorIntelTypeHierarchyViewModel type_hierarchy;
    UmiEditorNavigationInsightsSnapshot source_snapshot;
    uint64_t source_revision;
    uint64_t revision;
} UmiEditorIntelNavigationProjection;

UmiStatus umi_editor_intel_navigation_projection_init(
    UmiEditorIntelNavigationProjection *projection);
UmiStatus umi_editor_intel_navigation_projection_refresh(
    UmiEditorIntelNavigationProjection *projection,
    UmiEditorNavigationInsights *insights);
const UmiEditorIntelEntry *umi_editor_intel_navigation_projection_at(
    const UmiEditorIntelNavigationProjection *projection,
    size_t index);
size_t umi_editor_intel_navigation_projection_count(
    const UmiEditorIntelNavigationProjection *projection);
int umi_editor_intel_navigation_projection_valid(
    const UmiEditorIntelNavigationProjection *projection);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_NAVIGATION_PROJECTION_H */
