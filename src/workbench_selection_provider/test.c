/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/test.c
 *
 * PURPOSE:
 *   Preserve test IDs, suite hierarchy, framework, source URI/line, outcome, duration, labels and selection state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/test.h"

#include "umicom/workbench_selection/builders.h"

#include <stdio.h>
#include <string.h>

/* Provide the outcome text operation used by this module and its client applications. */
static const char *outcome_text(int outcome)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (outcome) {
    case 1: return "passed";
    case 2: return "failed";
    case 3: return "skipped";
    case 4: return "not-run";
    default: return "unknown";
    }
}

/* Provide the add text operation used by this module and its client applications. */
static UmiStatus add_text(
    UmiWorkbenchSelection *selection,
    const char *name,
    const char *value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_text(
        &field, value != NULL ? value : "");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

/* Provide the add boolean operation used by this module and its client applications. */
static UmiStatus add_boolean(
    UmiWorkbenchSelection *selection,
    const char *name,
    bool value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_boolean(&field, value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

/* Provide the add decimal operation used by this module and its client applications. */
static UmiStatus add_decimal(
    UmiWorkbenchSelection *selection,
    const char *name,
    double value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_decimal(&field, value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

/*
 * Provide the workbench selection provider from test item operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_from_test_item(
    const UmiTestPlatformItemSnapshot *item,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    char selection_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    const char *source_uri;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || application_id == NULL ||
        panel_id == NULL || out_selection == NULL ||
        item->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_make_id(
        selection_id,
        sizeof(selection_id),
        "test",
        item->id,
        item->revision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    source_uri = item->source_uri[0] != '\0'
        ? item->source_uri : item->uri;
    status = umi_workbench_selection_build_test(
        out_selection,
        selection_id,
        application_id,
        panel_id,
        workspace_id,
        item->id,
        item->suite_id,
        outcome_text(item->last_outcome),
        source_uri,
        item->source_line,
        (uint64_t)(item->last_duration_ms > 0.0
            ? item->last_duration_ms : 0.0),
        timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = add_text(out_selection, "parent-id", item->parent_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "name", item->name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "framework", item->framework);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "test-kind", item->kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "labels", item->labels);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "working-directory", item->working_directory);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "enabled", item->enabled != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "discovered", item->discovered != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_decimal(out_selection, "last-duration-ms", item->last_duration_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_workbench_selection_set_display_text(
        out_selection,
        item->name[0] != '\0' ? item->name : item->id);
    (void)umi_workbench_selection_refresh_hash(out_selection);
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection provider from test row operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_provider_from_test_row(
    const UmiTestExplorerRow *row,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (row == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_selection_provider_from_test_item(
        &row->item,
        application_id,
        panel_id,
        workspace_id,
        timestamp_ms,
        out_selection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    {
        UmiWorkbenchSelectionField field;
        umi_workbench_selection_field_init(&field, "selected");
        status = umi_workbench_selection_field_set_boolean(
            &field, row->selected != 0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_workbench_selection_add_field(
                out_selection, &field);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    {
        UmiWorkbenchSelectionField field;
        umi_workbench_selection_field_init(&field, "depth");
        status = umi_workbench_selection_field_set_unsigned(
            &field, (uint64_t)row->depth);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_workbench_selection_add_field(
                out_selection, &field);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    {
        UmiWorkbenchSelectionField field;
        umi_workbench_selection_field_init(&field, "child-count");
        status = umi_workbench_selection_field_set_unsigned(
            &field, (uint64_t)row->child_count);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_workbench_selection_add_field(
                out_selection, &field);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    out_selection->boolean_value = row->selected != 0;
    (void)umi_workbench_selection_refresh_hash(out_selection);
    return UMI_STATUS_OK;
}

/*
 * Find workbench selection provider test model while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_workbench_selection_provider_test_model_find(
    const UmiTestExplorerModel *model,
    const char *item_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    size_t count;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    count = umi_test_explorer_model_count(model);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiTestExplorerRow row;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_test_explorer_model_row_at(
                model, index, &row) == UMI_STATUS_OK &&
            strcmp(row.item.id, item_id) == 0) {
            return umi_workbench_selection_provider_from_test_row(
                &row,
                application_id,
                panel_id,
                workspace_id,
                timestamp_ms,
                out_selection);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find workbench selection provider test model visible while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_workbench_selection_provider_test_model_visible_at(
    const UmiTestExplorerModel *model,
    size_t visible_index,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    UmiTestExplorerRow row;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_explorer_model_visible_row_at(
        model, visible_index, &row);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_provider_from_test_row(
        &row,
        application_id,
        panel_id,
        workspace_id,
        timestamp_ms,
        out_selection);
}
