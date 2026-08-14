/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/semantic_clipboard.h
 *
 * PURPOSE:
 *   Copy and paste complete semantic component subtrees in the visual builder.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Clipboard entries are semantic nodes, not operating-system widget handles.
 * This keeps copy and paste deterministic in GTK4, web and headless hosts.
 */
#ifndef UMICOM_DESIGNER_SEMANTIC_CLIPBOARD_H
#define UMICOM_DESIGNER_SEMANTIC_CLIPBOARD_H

#include "umicom/designer/selection.h"
#include "umicom/designer/transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_SEMANTIC_CLIPBOARD_NODE_CAPACITY 64U

typedef struct UmiDesignerSemanticClipboard {
    UmiDeclNode nodes[UMI_DESIGNER_SEMANTIC_CLIPBOARD_NODE_CAPACITY];
    size_t node_count;
    uint64_t revision;
} UmiDesignerSemanticClipboard;

void umi_designer_clipboard_init(UmiDesignerSemanticClipboard *clipboard);
UmiStatus umi_designer_clipboard_copy(UmiDesignerSemanticClipboard *clipboard,
                                         const UmiDesignerDocument *document,
                                         const UmiDesignerSelection *selection);
UmiStatus umi_designer_clipboard_paste(const UmiDesignerSemanticClipboard *clipboard,
                                          UmiDesignerDocument *document,
                                          UmiDesignerTransactionHistory *history,
                                          const char *target_parent_id,
                                          const char *id_prefix);

#ifdef __cplusplus
}
#endif
#endif
