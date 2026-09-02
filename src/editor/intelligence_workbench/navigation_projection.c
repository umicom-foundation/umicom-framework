/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/navigation_projection.c
 *
 * PURPOSE:
 *   Implement the canonical source-navigation workbench projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/navigation_projection.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "umicom/editor/intelligence_workbench/projection.h"

/* Provide the location label operation used by this module and its client applications. */
static const char *location_label(const UmiEditorSourceLocation *location)
{
    const char *slash;
    const char *backslash;
    const char *label;

    /* Apply this branch only when its contract condition is satisfied. */
    if (location->label[0] != '\0') return location->label;
    label = location->uri;
    slash = strrchr(location->uri, '/');
    backslash = strrchr(location->uri, '\\');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (slash != NULL) label = slash + 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (backslash != NULL && backslash + 1 > label) label = backslash + 1;
    return label[0] == '\0' ? "Source target" : label;
}

/* Provide the navigation entry operation used by this module and its client applications. */
static UmiStatus navigation_entry(
    UmiEditorIntelEntry *entry,
    const UmiEditorNavigationResult *result,
    size_t index,
    int selected)
{
    char id[UMI_EDITOR_INTEL_ID_CAPACITY];
    const uint64_t hash =
        umi_editor_intel_projection_hash_text(result->location.uri) ^
        (result->sequence * UINT64_C(1099511628211)) ^
        umi_editor_intel_projection_hash_text(result->provider_id) ^
        (result->location.line * UINT64_C(1099511628211)) ^
        result->location.column;
    UmiEditorIntelProjectionFlags flags =
        UMI_EDITOR_INTEL_PROJECTION_VISIBLE;
    int written;

    written = snprintf(id, sizeof(id), "navigation-%016" PRIx64, hash);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (selected) flags |= UMI_EDITOR_INTEL_PROJECTION_SELECTED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result->primary) flags |= UMI_EDITOR_INTEL_PROJECTION_PRIMARY;
    return umi_editor_intel_projection_entry_from_source(
        entry, id, location_label(&result->location), result->detail,
        &result->location,
        umi_editor_intel_projection_score_from_rank(result->rank),
        flags, result->sequence == 0U ? (uint64_t)index + 1U : result->sequence);
}

/* Provide the hierarchy entry operation used by this module and its client applications. */
static UmiStatus hierarchy_entry(
    UmiEditorIntelEntry *entry,
    const UmiEditorSymbolHierarchyNode *node,
    uint64_t revision)
{
    char id[UMI_EDITOR_INTEL_ID_CAPACITY];
    UmiEditorIntelProjectionFlags flags = 0U;
    int written;

    written = snprintf(
        id, sizeof(id), "hierarchy-%016" PRIx64,
        umi_editor_intel_projection_hash_text(node->node_id));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (node->visible) flags |= UMI_EDITOR_INTEL_PROJECTION_VISIBLE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (node->selected) flags |= UMI_EDITOR_INTEL_PROJECTION_SELECTED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (node->expanded) flags |= UMI_EDITOR_INTEL_PROJECTION_EXPANDED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (node->cycle) flags |= UMI_EDITOR_INTEL_PROJECTION_CYCLE;
    return umi_editor_intel_projection_entry_from_source(
        entry, id, node->label, node->detail, &node->location,
        node->depth > (size_t)UINT32_MAX ? UINT32_MAX : (uint32_t)node->depth,
        flags, revision);
}

/* Provide the refresh peek operation used by this module and its client applications. */
static UmiStatus refresh_peek(
    UmiEditorIntelNavigationProjection *projection,
    UmiEditorNavigationInsights *insights)
{
    UmiEditorPeekDefinitionModel *model =
        umi_editor_navigation_insights_peek(insights);
    UmiEditorPeekDefinitionSnapshot snapshot;
    size_t index;
    size_t count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_editor_peek_definition_model_snapshot(model, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    count = snapshot.result_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_EDITOR_INTEL_MAX_ITEMS) count = UMI_EDITOR_INTEL_MAX_ITEMS;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiEditorNavigationResult result;
        UmiEditorIntelEntry entry;

        status = umi_editor_peek_definition_model_at(model, index, &result);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = navigation_entry(
            &entry, &result, index, index == snapshot.active_index);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_editor_intel_navigation_target_set_add(
            &projection->peek_targets, &entry);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    projection->source_revision = snapshot.revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the refresh references operation used by this module and its client
 * applications.
 */
static UmiStatus refresh_references(
    UmiEditorIntelNavigationProjection *projection,
    UmiEditorNavigationInsights *insights)
{
    UmiEditorReferenceResults *results =
        umi_editor_navigation_insights_references(insights);
    UmiEditorReferenceResultsSnapshot snapshot;
    size_t index;
    size_t count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_editor_reference_results_snapshot(results, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    count = snapshot.result_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_EDITOR_INTEL_MAX_ITEMS) count = UMI_EDITOR_INTEL_MAX_ITEMS;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiEditorReferenceEntry source_entry;
        UmiEditorIntelEntry entry;

        status = umi_editor_reference_results_entry_at(
            results, index, &source_entry);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = navigation_entry(
            &entry, &source_entry.result, index, source_entry.selected);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this operation only while the related capability or state is available. */
        if (!source_entry.visible) {
            entry.flags &=
                ~(uint32_t)UMI_EDITOR_INTEL_PROJECTION_VISIBLE;
        }
        status = umi_editor_intel_reference_results_model_add(
            &projection->references, &entry);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    projection->source_revision = snapshot.revision;
    return UMI_STATUS_OK;
}

/* Provide the refresh hierarchy operation used by this module and its client applications. */
static UmiStatus refresh_hierarchy(
    UmiEditorIntelNavigationProjection *projection,
    UmiEditorSymbolHierarchy *hierarchy,
    int call_hierarchy)
{
    UmiEditorSymbolHierarchySnapshot snapshot;
    size_t index;
    size_t count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_editor_symbol_hierarchy_snapshot(hierarchy, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    count = snapshot.node_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_EDITOR_INTEL_MAX_ITEMS) count = UMI_EDITOR_INTEL_MAX_ITEMS;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiEditorSymbolHierarchyNode node;
        UmiEditorIntelEntry entry;

        status = umi_editor_symbol_hierarchy_at(hierarchy, index, &node);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = hierarchy_entry(&entry, &node, snapshot.revision);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = call_hierarchy
            ? umi_editor_intel_call_hierarchy_view_model_add(
                  &projection->call_hierarchy, &entry)
            : umi_editor_intel_type_hierarchy_view_model_add(
                  &projection->type_hierarchy, &entry);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    projection->source_revision = snapshot.revision;
    return UMI_STATUS_OK;
}

/*
 * Initialise editor intel navigation projection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_navigation_projection_init(
    UmiEditorIntelNavigationProjection *projection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(projection, 0, sizeof(*projection));
    projection->struct_size = (uint32_t)sizeof(*projection);
    projection->api_version =
        UMI_EDITOR_INTEL_NAVIGATION_PROJECTION_API_VERSION;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_intel_navigation_target_set_init(
            &projection->peek_targets) != UMI_STATUS_OK ||
        umi_editor_intel_reference_results_model_init(
            &projection->references) != UMI_STATUS_OK ||
        umi_editor_intel_call_hierarchy_view_model_init(
            &projection->call_hierarchy) != UMI_STATUS_OK ||
        umi_editor_intel_type_hierarchy_view_model_init(
            &projection->type_hierarchy) != UMI_STATUS_OK) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    projection->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor intel navigation projection refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_navigation_projection_refresh(
    UmiEditorIntelNavigationProjection *projection,
    UmiEditorNavigationInsights *insights)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || insights == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_navigation_insights_snapshot(
        insights, &projection->source_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)umi_editor_intel_navigation_target_set_clear(
        &projection->peek_targets);
    (void)umi_editor_intel_reference_results_model_clear(
        &projection->references);
    (void)umi_editor_intel_call_hierarchy_view_model_clear(
        &projection->call_hierarchy);
    (void)umi_editor_intel_type_hierarchy_view_model_clear(
        &projection->type_hierarchy);

    /* Select the behaviour associated with the requested command or state value. */
    switch (projection->source_snapshot.active_surface) {
        case UMI_EDITOR_NAVIGATION_INSIGHT_NONE:
            projection->source_revision = projection->source_snapshot.revision;
            status = UMI_STATUS_OK;
            break;
        case UMI_EDITOR_NAVIGATION_INSIGHT_PEEK_DEFINITION:
            status = refresh_peek(projection, insights);
            break;
        case UMI_EDITOR_NAVIGATION_INSIGHT_REFERENCES:
            status = refresh_references(projection, insights);
            break;
        case UMI_EDITOR_NAVIGATION_INSIGHT_CALL_HIERARCHY:
            status = refresh_hierarchy(
                projection,
                umi_editor_navigation_insights_call_hierarchy(insights), 1);
            break;
        case UMI_EDITOR_NAVIGATION_INSIGHT_TYPE_HIERARCHY:
            status = refresh_hierarchy(
                projection,
                umi_editor_navigation_insights_type_hierarchy(insights), 0);
            break;
        default:
            status = UMI_STATUS_INVALID_STATE;
            break;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++projection->revision;
    return status;
}

/*
 * Find editor intel navigation projection while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_navigation_projection_at(
    const UmiEditorIntelNavigationProjection *projection,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL) return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch (projection->source_snapshot.active_surface) {
        case UMI_EDITOR_NAVIGATION_INSIGHT_PEEK_DEFINITION:
            return index < projection->peek_targets.count
                ? &projection->peek_targets.items[index] : NULL;
        case UMI_EDITOR_NAVIGATION_INSIGHT_REFERENCES:
            return index < projection->references.count
                ? &projection->references.items[index] : NULL;
        case UMI_EDITOR_NAVIGATION_INSIGHT_CALL_HIERARCHY:
            return index < projection->call_hierarchy.count
                ? &projection->call_hierarchy.items[index] : NULL;
        case UMI_EDITOR_NAVIGATION_INSIGHT_TYPE_HIERARCHY:
            return index < projection->type_hierarchy.count
                ? &projection->type_hierarchy.items[index] : NULL;
        default:
            return NULL;
    }
}

/*
 * Return the number of records represented by editor intel navigation projection without
 * changing their state.
 */
size_t umi_editor_intel_navigation_projection_count(
    const UmiEditorIntelNavigationProjection *projection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL) return 0U;
    /* Select the behaviour associated with the requested command or state value. */
    switch (projection->source_snapshot.active_surface) {
        case UMI_EDITOR_NAVIGATION_INSIGHT_PEEK_DEFINITION:
            return projection->peek_targets.count;
        case UMI_EDITOR_NAVIGATION_INSIGHT_REFERENCES:
            return projection->references.count;
        case UMI_EDITOR_NAVIGATION_INSIGHT_CALL_HIERARCHY:
            return projection->call_hierarchy.count;
        case UMI_EDITOR_NAVIGATION_INSIGHT_TYPE_HIERARCHY:
            return projection->type_hierarchy.count;
        default:
            return 0U;
    }
}

/*
 * Check that editor intel navigation projection satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_navigation_projection_valid(
    const UmiEditorIntelNavigationProjection *projection)
{
    return projection != NULL &&
        projection->struct_size == (uint32_t)sizeof(*projection) &&
        projection->api_version ==
            UMI_EDITOR_INTEL_NAVIGATION_PROJECTION_API_VERSION &&
        umi_editor_intel_navigation_target_set_valid(
            &projection->peek_targets) &&
        umi_editor_intel_reference_results_model_valid(
            &projection->references) &&
        umi_editor_intel_call_hierarchy_view_model_valid(
            &projection->call_hierarchy) &&
        umi_editor_intel_type_hierarchy_view_model_valid(
            &projection->type_hierarchy);
}
