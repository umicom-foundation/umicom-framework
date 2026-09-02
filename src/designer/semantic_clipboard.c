/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/semantic_clipboard.c
 *
 * PURPOSE:
 *   Capture semantic subtrees and paste remapped copies as one undoable action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/semantic_clipboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initialise designer clipboard from caller-provided values so later operations receive a
 * known state.
 */
void umi_designer_clipboard_init(UmiDesignerSemanticClipboard *clipboard)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (clipboard != NULL) (void)memset(clipboard, 0, sizeof(*clipboard));
}

/* Provide the contains operation used by this module and its client applications. */
static int contains(const UmiDesignerSemanticClipboard *clipboard, const char *node_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < clipboard->node_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(clipboard->nodes[index].node_id, node_id) == 0) return 1;
    }
    return 0;
}

/*
 * Copy designer clipboard into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_designer_clipboard_copy(UmiDesignerSemanticClipboard *clipboard,
                                         const UmiDesignerDocument *document,
                                         const UmiDesignerSelection *selection)
{
    UmiDeclDocument *semantic;
    size_t pass;
    size_t index;
    int changed = 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (clipboard == NULL || document == NULL || selection == NULL || selection->count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    semantic = umi_designer_document_declarative((UmiDesignerDocument *)document);
    clipboard->node_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        UmiDeclNode node;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_decl_document_find_node(semantic, selection->node_ids[index], &node) != UMI_STATUS_OK) return UMI_STATUS_NOT_FOUND;
        clipboard->nodes[clipboard->node_count++] = node;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (pass = 0U; changed != 0 && pass < umi_decl_document_node_count(semantic); ++pass) {
        changed = 0;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < umi_decl_document_node_count(semantic); ++index) {
            UmiDeclNode node;
            (void)umi_decl_document_node_at(semantic, index, &node);
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (!contains(clipboard, node.node_id) && contains(clipboard, node.parent_id)) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (clipboard->node_count >= UMI_DESIGNER_SEMANTIC_CLIPBOARD_NODE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
                clipboard->nodes[clipboard->node_count++] = node;
                changed = 1;
            }
        }
    }
    clipboard->revision += 1U;
    return UMI_STATUS_OK;
}

/* Provide the make identifier operation used by this module and its client applications. */
static UmiStatus make_identifier(char *destination, size_t capacity, const char *prefix, const char *original)
{
    int written = snprintf(destination, capacity, "%s_%s", prefix, original);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/*
 * Provide the designer clipboard paste operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_clipboard_paste(const UmiDesignerSemanticClipboard *clipboard,
                                          UmiDesignerDocument *document,
                                          UmiDesignerTransactionHistory *history,
                                          const char *target_parent_id,
                                          const char *id_prefix)
{
    UmiDesignerTransaction *transaction;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (clipboard == NULL || document == NULL || history == NULL || clipboard->node_count == 0U ||
        target_parent_id == NULL || id_prefix == NULL || id_prefix[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    transaction = (UmiDesignerTransaction *)malloc(sizeof(*transaction));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_designer_transaction_init(transaction, "clipboard-paste", "Paste component subtree");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < clipboard->node_count; ++index) {
        UmiDeclNode node = clipboard->nodes[index];
        UmiDesignerOperation operation;
        size_t parent_index;
        char new_id[UMI_DECL_ID_CAPACITY];
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        status = make_identifier(new_id, sizeof(new_id), id_prefix, node.node_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        status = umi_decl_copy_text(node.node_id, sizeof(node.node_id), new_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        /* Visit each bounded item once so every record receives the same rule. */
        for (parent_index = 0U; parent_index < clipboard->node_count; ++parent_index) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(node.parent_id, clipboard->nodes[parent_index].node_id) == 0) {
                char remapped[UMI_DECL_ID_CAPACITY];
                status = make_identifier(remapped, sizeof(remapped), id_prefix, node.parent_id);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    status = umi_decl_copy_text(node.parent_id, sizeof(node.parent_id), remapped);
                }
                break;
            }
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK && parent_index == clipboard->node_count) {
            status = umi_decl_copy_text(node.parent_id, sizeof(node.parent_id), target_parent_id);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_designer_operation_add(&node, &operation);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_designer_transaction_add(transaction, &operation);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_designer_transaction_history_execute(history, transaction);
    }
    free(transaction);
    return status;
}
