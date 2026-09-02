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

/**
 * Represent the editor intel navigation projection data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise editor intel navigation projection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_navigation_projection_init(
    UmiEditorIntelNavigationProjection *projection);
/**
 * Provide the editor intel navigation projection refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_navigation_projection_refresh(
    UmiEditorIntelNavigationProjection *projection,
    UmiEditorNavigationInsights *insights);
/**
 * Find editor intel navigation projection while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_navigation_projection_at(
    const UmiEditorIntelNavigationProjection *projection,
    size_t index);
/**
 * Return the number of records represented by editor intel navigation projection without
 * changing their state.
 */
size_t umi_editor_intel_navigation_projection_count(
    const UmiEditorIntelNavigationProjection *projection);
/**
 * Check that editor intel navigation projection satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_navigation_projection_valid(
    const UmiEditorIntelNavigationProjection *projection);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_NAVIGATION_PROJECTION_H */
