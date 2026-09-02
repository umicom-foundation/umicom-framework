/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/drag_drop.c
 * PURPOSE: Implement palette insertion and hierarchy moves for Builder v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/drag_drop.h"

#include <string.h>

/* Provide the is descendant operation used by this module and its client applications. */
static int is_descendant(UmiDeclDocument *document,
                         const char *candidate_parent,
                         const char *source_node)
{
    char current_id[UMI_DECL_ID_CAPACITY];
    size_t remaining = umi_decl_document_node_count(document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_decl_copy_text(current_id, sizeof(current_id), candidate_parent) != UMI_STATUS_OK) return 1;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (remaining-- > 0U && current_id[0] != '\0' && strcmp(current_id, "-") != 0) {
        UmiDeclNode current;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(current_id, source_node) == 0) return 1;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_decl_document_find_node(document, current_id, &current) != UMI_STATUS_OK) return 0;
        (void)umi_decl_copy_text(current_id, sizeof(current_id), current.parent_id);
    }
    return 0;
}

/* Provide the initialise operation used by this module and its client applications. */
static UmiStatus initialise(UmiDesignerDragDrop *drag)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(drag, 0, sizeof(*drag));
    drag->active = 1;
    drag->position = UMI_DESIGNER_DROP_INTO;
    return UMI_STATUS_OK;
}

/*
 * Provide the designer drag drop begin palette operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_drag_drop_begin_palette(
    UmiDesignerDragDrop *drag,
    const char *component_type,
    const char *proposed_node_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (component_type == NULL || proposed_node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = initialise(drag);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_copy_text(
        drag->component_type, sizeof(drag->component_type), component_type);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_copy_text(
        drag->proposed_node_id, sizeof(drag->proposed_node_id), proposed_node_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) drag->from_palette = 1;
    return status;
}

/*
 * Provide the designer drag drop begin node operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_drag_drop_begin_node(
    UmiDesignerDragDrop *drag,
    const char *node_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = initialise(drag);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_copy_text(
        drag->source_node_id, sizeof(drag->source_node_id), node_id);
    return status;
}

/*
 * Provide the designer drag drop target operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_drag_drop_target(
    UmiDesignerDragDrop *drag,
    const char *target_node_id,
    UmiDesignerDropPosition position)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag == NULL || !drag->active || target_node_id == NULL ||
        position < UMI_DESIGNER_DROP_INTO || position > UMI_DESIGNER_DROP_AFTER ||
        (!drag->from_palette && strcmp(drag->source_node_id, target_node_id) == 0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    drag->position = position;
    return umi_decl_copy_text(drag->target_node_id,
                              sizeof(drag->target_node_id), target_node_id);
}

/*
 * Provide the designer drag drop commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_drag_drop_commit(
    UmiDesignerDragDrop *drag,
    UmiDesignerHistory *history,
    UmiDesignerDocument *document)
{
    UmiDeclDocument *declarative;
    UmiDeclNode target;
    UmiDesignerOperation operation;
    UmiStatus status;
    const char *parent_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag == NULL || history == NULL || document == NULL || !drag->active ||
        drag->target_node_id[0] == '\0') return UMI_STATUS_INVALID_STATE;
    declarative = umi_designer_document_declarative(document);
    status = umi_decl_document_find_node(declarative, drag->target_node_id, &target);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    parent_id = drag->position == UMI_DESIGNER_DROP_INTO
        ? target.node_id : target.parent_id;
    /* Apply this branch only when its contract condition is satisfied. */
    if (drag->from_palette) {
        UmiDeclNode node;
        status = umi_decl_node_init(&node, drag->proposed_node_id,
                                    drag->component_type, parent_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_designer_operation_add(
            &node, &operation);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        UmiDeclNode before;
        UmiDeclNode after;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (is_descendant(declarative, parent_id, drag->source_node_id)) {
            return UMI_STATUS_INVALID_STATE;
        }
        status = umi_decl_document_find_node(
            declarative, drag->source_node_id, &before);
        after = before;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_decl_copy_text(
            after.parent_id, sizeof(after.parent_id), parent_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_designer_operation_move(
            &before, &after, &operation);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_history_execute(
        history, &operation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) drag->active = 0;
    return status;
}

/*
 * Provide the designer drag drop cancel operation used by this module and its client
 * applications.
 */
void umi_designer_drag_drop_cancel(UmiDesignerDragDrop *drag)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag != NULL) (void)memset(drag, 0, sizeof(*drag));
}
