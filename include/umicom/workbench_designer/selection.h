/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/selection.h
 *
 * PURPOSE:
 *   Maintain deterministic multi-selection and one primary node without storing
 *   toolkit objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_SELECTION_H
#define UMICOM_WORKBENCH_DESIGNER_SELECTION_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerSelection {
    UmiWorkbenchDesignerIdentifier items[UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS];
    size_t count;
    char primary_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint64_t revision;
} UmiWorkbenchDesignerSelection;

void umi_workbench_designer_selection_init(UmiWorkbenchDesignerSelection *selection);
void umi_workbench_designer_selection_clear(UmiWorkbenchDesignerSelection *selection);
UmiStatus umi_workbench_designer_selection_add(UmiWorkbenchDesignerSelection *selection, const char *node_id, bool make_primary);
UmiStatus umi_workbench_designer_selection_remove(UmiWorkbenchDesignerSelection *selection, const char *node_id);
UmiStatus umi_workbench_designer_selection_toggle(UmiWorkbenchDesignerSelection *selection, const char *node_id, bool make_primary);
UmiStatus umi_workbench_designer_selection_set_primary(UmiWorkbenchDesignerSelection *selection, const char *node_id);
bool umi_workbench_designer_selection_contains(const UmiWorkbenchDesignerSelection *selection, const char *node_id);
const char *umi_workbench_designer_selection_at(const UmiWorkbenchDesignerSelection *selection, size_t index);
const char *umi_workbench_designer_selection_primary(const UmiWorkbenchDesignerSelection *selection);

#ifdef __cplusplus
}
#endif

#endif
