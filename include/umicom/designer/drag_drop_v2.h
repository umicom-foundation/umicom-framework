/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/drag_drop_v2.h
 * PURPOSE: Model palette and hierarchy drag/drop as undoable semantic edits.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Pointer motion stays in the frontend. Framework receives only
 * stable source, target and position data when the user completes a drop. */
#ifndef UMICOM_DESIGNER_DRAG_DROP_V2_H
#define UMICOM_DESIGNER_DRAG_DROP_V2_H

#include "umicom/designer/builder_types_v2.h"
#include "umicom/designer/history.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerDragDropV2 {
    char source_node_id[UMI_DECL_ID_CAPACITY];
    char component_type[UMI_DECL_ID_CAPACITY];
    char proposed_node_id[UMI_DECL_ID_CAPACITY];
    char target_node_id[UMI_DECL_ID_CAPACITY];
    UmiDesignerDropPositionV2 position;
    int from_palette;
    int active;
} UmiDesignerDragDropV2;

UmiStatus umi_designer_drag_drop_v2_begin_palette(
    UmiDesignerDragDropV2 *drag,
    const char *component_type,
    const char *proposed_node_id);
UmiStatus umi_designer_drag_drop_v2_begin_node(
    UmiDesignerDragDropV2 *drag,
    const char *node_id);
UmiStatus umi_designer_drag_drop_v2_target(
    UmiDesignerDragDropV2 *drag,
    const char *target_node_id,
    UmiDesignerDropPositionV2 position);
UmiStatus umi_designer_drag_drop_v2_commit(
    UmiDesignerDragDropV2 *drag,
    UmiDesignerHistory *history,
    UmiDesignerDocument *document);
void umi_designer_drag_drop_v2_cancel(UmiDesignerDragDropV2 *drag);

#ifdef __cplusplus
}
#endif

#endif
