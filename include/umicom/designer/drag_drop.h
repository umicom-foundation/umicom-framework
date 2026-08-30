/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/drag_drop.h
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
#ifndef UMICOM_DESIGNER_DRAG_DROP_H
#define UMICOM_DESIGNER_DRAG_DROP_H

#include "umicom/designer/builder_types.h"
#include "umicom/designer/history.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerDragDrop {
    char source_node_id[UMI_DECL_ID_CAPACITY];
    char component_type[UMI_DECL_ID_CAPACITY];
    char proposed_node_id[UMI_DECL_ID_CAPACITY];
    char target_node_id[UMI_DECL_ID_CAPACITY];
    UmiDesignerDropPosition position;
    int from_palette;
    int active;
} UmiDesignerDragDrop;

UmiStatus umi_designer_drag_drop_begin_palette(
    UmiDesignerDragDrop *drag,
    const char *component_type,
    const char *proposed_node_id);
UmiStatus umi_designer_drag_drop_begin_node(
    UmiDesignerDragDrop *drag,
    const char *node_id);
UmiStatus umi_designer_drag_drop_target(
    UmiDesignerDragDrop *drag,
    const char *target_node_id,
    UmiDesignerDropPosition position);
UmiStatus umi_designer_drag_drop_commit(
    UmiDesignerDragDrop *drag,
    UmiDesignerHistory *history,
    UmiDesignerDocument *document);
void umi_designer_drag_drop_cancel(UmiDesignerDragDrop *drag);

#ifdef __cplusplus
}
#endif

#endif
