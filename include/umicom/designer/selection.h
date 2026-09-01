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
typedef struct UmiDesignerSelection { char node_ids[UMI_DESIGNER_MAX_SELECTION][UMI_DECL_ID_CAPACITY]; size_t count; char primary[UMI_DECL_ID_CAPACITY]; } UmiDesignerSelection;
void umi_designer_selection_clear(UmiDesignerSelection *selection);
UmiStatus umi_designer_selection_set_primary(UmiDesignerSelection *selection,const char *node_id);
UmiStatus umi_designer_selection_add(UmiDesignerSelection *selection,const char *node_id);
int umi_designer_selection_contains(const UmiDesignerSelection *selection,const char *node_id);
#ifdef __cplusplus
}
#endif
#endif
