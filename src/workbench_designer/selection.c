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


void umi_workbench_designer_selection_init(UmiWorkbenchDesignerSelection *selection)
{
    if (selection == NULL) return;
    (void)memset(selection, 0, sizeof(*selection));
}

void umi_workbench_designer_selection_clear(UmiWorkbenchDesignerSelection *selection)
{
    if (selection == NULL) return;
    selection->count = 0U;
    selection->primary_node_id[0] = '\0';
    selection->revision += 1U;
}

bool umi_workbench_designer_selection_contains(
    const UmiWorkbenchDesignerSelection *selection,
    const char *node_id)
{
    size_t index;
    if (selection == NULL || node_id == NULL || node_id[0] == '\0') return false;
    for (index = 0U; index < selection->count; ++index) {
        if (strcmp(selection->items[index].value, node_id) == 0) return true;
    }
    return false;
}

UmiStatus umi_workbench_designer_selection_add(
    UmiWorkbenchDesignerSelection *selection,
    const char *node_id,
    bool make_primary)
{
    UmiStatus status;
    if (selection == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_designer_selection_contains(selection, node_id)) {
        if (selection->count >= UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = umi_workbench_designer_copy_text(
            selection->items[selection->count].value,
            sizeof(selection->items[selection->count].value),
            node_id);
        if (status != UMI_STATUS_OK) return status;
        selection->count += 1U;
    }
    if (make_primary || selection->primary_node_id[0] == '\0') {
        status = umi_workbench_designer_copy_text(
            selection->primary_node_id,
            sizeof(selection->primary_node_id),
            node_id);
        if (status != UMI_STATUS_OK) return status;
    }
    selection->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_selection_remove(
    UmiWorkbenchDesignerSelection *selection,
    const char *node_id)
{
    size_t index;
    if (selection == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < selection->count; ++index) {
        if (strcmp(selection->items[index].value, node_id) == 0) {
            size_t move_index;
            for (move_index = index + 1U;
                 move_index < selection->count;
                 ++move_index) {
                selection->items[move_index - 1U] = selection->items[move_index];
            }
            selection->count -= 1U;
            if (strcmp(selection->primary_node_id, node_id) == 0) {
                if (selection->count == 0U) selection->primary_node_id[0] = '\0';
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

UmiStatus umi_workbench_designer_selection_toggle(
    UmiWorkbenchDesignerSelection *selection,
    const char *node_id,
    bool make_primary)
{
    if (umi_workbench_designer_selection_contains(selection, node_id)) {
        return umi_workbench_designer_selection_remove(selection, node_id);
    }
    return umi_workbench_designer_selection_add(selection, node_id, make_primary);
}

UmiStatus umi_workbench_designer_selection_set_primary(
    UmiWorkbenchDesignerSelection *selection,
    const char *node_id)
{
    if (selection == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_workbench_designer_selection_contains(selection, node_id)) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(selection->primary_node_id, node_id) != 0) {
        UmiStatus status = umi_workbench_designer_copy_text(
            selection->primary_node_id,
            sizeof(selection->primary_node_id),
            node_id);
        if (status != UMI_STATUS_OK) return status;
        selection->revision += 1U;
    }
    return UMI_STATUS_OK;
}

const char *umi_workbench_designer_selection_at(
    const UmiWorkbenchDesignerSelection *selection,
    size_t index)
{
    if (selection == NULL || index >= selection->count) return NULL;
    return selection->items[index].value;
}

const char *umi_workbench_designer_selection_primary(
    const UmiWorkbenchDesignerSelection *selection)
{
    if (selection == NULL || selection->primary_node_id[0] == '\0') return NULL;
    return selection->primary_node_id;
}
