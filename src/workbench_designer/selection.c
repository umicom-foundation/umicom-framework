/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/selection.c
 *
 * PURPOSE:
 *   Implement bounded selection changes with stable revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/selection.h"
#include "internal.h"


/*
 * Initialise workbench designer selection from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_selection_init(UmiWorkbenchDesignerSelection *selection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return;
    (void)memset(selection, 0, sizeof(*selection));
}

/*
 * Release or reset state held by workbench designer selection so the same storage can be
 * reused safely.
 */
void umi_workbench_designer_selection_clear(UmiWorkbenchDesignerSelection *selection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return;
    selection->count = 0U;
    selection->primary_node_id[0] = '\0';
    selection->revision += 1U;
}

/*
 * Provide the workbench designer selection contains operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_selection_contains(
    const UmiWorkbenchDesignerSelection *selection,
    const char *node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || node_id == NULL || node_id[0] == '\0') return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(selection->items[index].value, node_id) == 0) return true;
    }
    return false;
}

/*
 * Add workbench designer selection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_designer_selection_add(
    UmiWorkbenchDesignerSelection *selection,
    const char *node_id,
    bool make_primary)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_workbench_designer_selection_contains(selection, node_id)) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (selection->count >= UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = umi_workbench_designer_copy_text(
            selection->items[selection->count].value,
            sizeof(selection->items[selection->count].value),
            node_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        selection->count += 1U;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (make_primary || selection->primary_node_id[0] == '\0') {
        status = umi_workbench_designer_copy_text(
            selection->primary_node_id,
            sizeof(selection->primary_node_id),
            node_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    selection->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench designer selection while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_workbench_designer_selection_remove(
    UmiWorkbenchDesignerSelection *selection,
    const char *node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(selection->items[index].value, node_id) == 0) {
            size_t move_index;
            /* Visit each bounded item once so every record receives the same rule. */
            for (move_index = index + 1U;
                 move_index < selection->count;
                 ++move_index) {
                selection->items[move_index - 1U] = selection->items[move_index];
            }
            selection->count -= 1U;
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(selection->primary_node_id, node_id) == 0) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (selection->count == 0U) selection->primary_node_id[0] = '\0';
                /* Use this fallback path when the earlier condition does not apply. */
                else (void)umi_workbench_designer_copy_text(
                    selection->primary_node_id,
                    sizeof(selection->primary_node_id),
                    selection->items[0].value);
            }
            selection->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the workbench designer selection toggle operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_selection_toggle(
    UmiWorkbenchDesignerSelection *selection,
    const char *node_id,
    bool make_primary)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_workbench_designer_selection_contains(selection, node_id)) {
        return umi_workbench_designer_selection_remove(selection, node_id);
    }
    return umi_workbench_designer_selection_add(selection, node_id, make_primary);
}

/*
 * Provide the workbench designer selection set primary operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_selection_set_primary(
    UmiWorkbenchDesignerSelection *selection,
    const char *node_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_workbench_designer_selection_contains(selection, node_id)) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(selection->primary_node_id, node_id) != 0) {
        UmiStatus status = umi_workbench_designer_copy_text(
            selection->primary_node_id,
            sizeof(selection->primary_node_id),
            node_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        selection->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Find workbench designer selection while leaving the underlying catalogue or model owned
 * by this module.
 */
const char *umi_workbench_designer_selection_at(
    const UmiWorkbenchDesignerSelection *selection,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || index >= selection->count) return NULL;
    return selection->items[index].value;
}

/*
 * Provide the workbench designer selection primary operation used by this module and its
 * client applications.
 */
const char *umi_workbench_designer_selection_primary(
    const UmiWorkbenchDesignerSelection *selection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || selection->primary_node_id[0] == '\0') return NULL;
    return selection->primary_node_id;
}
