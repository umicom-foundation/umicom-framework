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


/**
 * Represent the workbench designer selection data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerSelection {
    UmiWorkbenchDesignerIdentifier items[UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS];
    size_t count;
    char primary_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint64_t revision;
} UmiWorkbenchDesignerSelection;

/**
 * Initialise workbench designer selection from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_selection_init(UmiWorkbenchDesignerSelection *selection);
/**
 * Release or reset state held by workbench designer selection so the same storage can be
 * reused safely.
 */
void umi_workbench_designer_selection_clear(UmiWorkbenchDesignerSelection *selection);
/**
 * Add workbench designer selection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_designer_selection_add(UmiWorkbenchDesignerSelection *selection, const char *node_id, bool make_primary);
/**
 * Remove workbench designer selection while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_workbench_designer_selection_remove(UmiWorkbenchDesignerSelection *selection, const char *node_id);
/**
 * Provide the workbench designer selection toggle operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_selection_toggle(UmiWorkbenchDesignerSelection *selection, const char *node_id, bool make_primary);
/**
 * Provide the workbench designer selection set primary operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_selection_set_primary(UmiWorkbenchDesignerSelection *selection, const char *node_id);
/**
 * Provide the workbench designer selection contains operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_selection_contains(const UmiWorkbenchDesignerSelection *selection, const char *node_id);
/**
 * Find workbench designer selection while leaving the underlying catalogue or model owned
 * by this module.
 */
const char *umi_workbench_designer_selection_at(const UmiWorkbenchDesignerSelection *selection, size_t index);
/**
 * Provide the workbench designer selection primary operation used by this module and its
 * client applications.
 */
const char *umi_workbench_designer_selection_primary(const UmiWorkbenchDesignerSelection *selection);

#ifdef __cplusplus
}
#endif

#endif
