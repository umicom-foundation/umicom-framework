/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/selection.h
 *
 * PURPOSE:
 *   Track stable semantic component selection independently from a tree view or GTK selection object.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_SELECTION_H
#define UMICOM_DESIGNER_SELECTION_H
#include "umicom/designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the designer selection data shared with callers of this public contract.
 */
typedef struct UmiDesignerSelection { char node_ids[UMI_DESIGNER_MAX_SELECTION][UMI_DECL_ID_CAPACITY]; size_t count; char primary[UMI_DECL_ID_CAPACITY]; } UmiDesignerSelection;
/**
 * Release or reset state held by designer selection so the same storage can be reused
 * safely.
 */
void umi_designer_selection_clear(UmiDesignerSelection *selection);
/**
 * Provide the designer selection set primary operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_selection_set_primary(UmiDesignerSelection *selection,const char *node_id);
/**
 * Add designer selection only after its inputs and available capacity have been checked.
 */
UmiStatus umi_designer_selection_add(UmiDesignerSelection *selection,const char *node_id);
/**
 * Provide the designer selection contains operation used by this module and its client
 * applications.
 */
int umi_designer_selection_contains(const UmiDesignerSelection *selection,const char *node_id);
#ifdef __cplusplus
}
#endif
#endif
