/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/layout.c
 *
 * PURPOSE:
 *   Turn reusable geometry calculations into atomic semantic layout commands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/layout.h"

#include <stdio.h>

static UmiStatus node_with_rect(UmiDesignerDocument *document,
                                const char *node_id,
                                UmiDesignerRect rect,
                                UmiDeclNode *out_before,
                                UmiDeclNode *out_after)
{
    char value[32];
    UmiStatus status = umi_decl_document_find_node(umi_designer_document_declarative(document), node_id, out_before);
    if (status != UMI_STATUS_OK) return status;
    *out_after = *out_before;
    (void)snprintf(value, sizeof(value), "%d", rect.x);
    status = umi_decl_node_set_attribute(out_after, "x", UMI_DECL_VALUE_INTEGER, value);
    if (status == UMI_STATUS_OK) {
        (void)snprintf(value, sizeof(value), "%d", rect.y);
        status = umi_decl_node_set_attribute(out_after, "y", UMI_DECL_VALUE_INTEGER, value);
    }
    if (status == UMI_STATUS_OK) {
        (void)snprintf(value, sizeof(value), "%d", rect.width);
        status = umi_decl_node_set_attribute(out_after, "width", UMI_DECL_VALUE_INTEGER, value);
    }
    if (status == UMI_STATUS_OK) {
        (void)snprintf(value, sizeof(value), "%d", rect.height);
        status = umi_decl_node_set_attribute(out_after, "height", UMI_DECL_VALUE_INTEGER, value);
    }
    return status;
}

static UmiStatus commit_rects(UmiDesignerDocument *document,
                              UmiDesignerTransactionHistory *history,
                              const UmiDesignerSelection *selection,
                              const UmiDesignerRect *rects,
                              const char *identifier,
                              const char *summary)
{
    UmiDesignerTransaction transaction;
    UmiStatus status;
    size_t index;
    status = umi_designer_transaction_init(&transaction, identifier, summary);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < selection->count; ++index) {
        UmiDeclNode before;
        UmiDeclNode after;
        UmiDesignerOperation operation;
        status = node_with_rect(document, selection->node_ids[index], rects[index], &before, &after);
        if (status == UMI_STATUS_OK) status = umi_designer_operation_move(&before, &after, &operation);
        if (status == UMI_STATUS_OK) status = umi_designer_transaction_add(&transaction, &operation);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_designer_transaction_history_execute(history, &transaction);
}

static UmiStatus read_rects(UmiDesignerDocument *document,
                            const UmiDesignerSelection *selection,
                            UmiDesignerRect *rects)
{
    size_t index;
    UmiStatus status;
    if (document == NULL || selection == NULL || rects == NULL || selection->count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < selection->count; ++index) {
        status = umi_designer_surface_get_rect(document, selection->node_ids[index], &rects[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_layout_align(UmiDesignerDocument *document,
                                       UmiDesignerTransactionHistory *history,
                                       const UmiDesignerSelection *selection,
                                       UmiDesignerAlignment alignment)
{
    UmiDesignerRect rects[UMI_DESIGNER_MAX_SELECTION];
    UmiStatus status;
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = read_rects(document, selection, rects);
    if (status == UMI_STATUS_OK) status = umi_designer_surface_align(rects, selection->count, alignment);
    if (status == UMI_STATUS_OK) status = commit_rects(document, history, selection, rects, "layout-align", "Align selected components");
    return status;
}

UmiStatus umi_designer_layout_distribute(UmiDesignerDocument *document,
                                            UmiDesignerTransactionHistory *history,
                                            const UmiDesignerSelection *selection,
                                            UmiDesignerDistribution distribution)
{
    UmiDesignerRect rects[UMI_DESIGNER_MAX_SELECTION];
    UmiStatus status;
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = read_rects(document, selection, rects);
    if (status == UMI_STATUS_OK) status = umi_designer_surface_distribute(rects, selection->count, distribution);
    if (status == UMI_STATUS_OK) status = commit_rects(document, history, selection, rects, "layout-distribute", "Distribute selected components");
    return status;
}

UmiStatus umi_designer_layout_snap(UmiDesignerDocument *document,
                                      UmiDesignerTransactionHistory *history,
                                      const UmiDesignerSelection *selection,
                                      const UmiDesignerSurfaceOptions *options)
{
    UmiDesignerRect rects[UMI_DESIGNER_MAX_SELECTION];
    UmiStatus status;
    size_t index;
    if (history == NULL || options == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = read_rects(document, selection, rects);
    for (index = 0U; status == UMI_STATUS_OK && index < selection->count; ++index) {
        rects[index] = umi_designer_surface_snap_rect(options, rects[index]);
    }
    if (status == UMI_STATUS_OK) status = commit_rects(document, history, selection, rects, "layout-snap", "Snap selected components to grid");
    return status;
}
