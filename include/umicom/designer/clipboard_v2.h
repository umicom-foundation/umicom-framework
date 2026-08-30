/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/clipboard_v2.h
 *
 * PURPOSE:
 *   Copy and paste complete semantic component subtrees in the visual builder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Clipboard entries are semantic nodes, not operating-system widget handles.
 * This keeps copy and paste deterministic in GTK4, web and headless hosts.
 */
#ifndef UMICOM_DESIGNER_CLIPBOARD_V2_H
#define UMICOM_DESIGNER_CLIPBOARD_V2_H

#include "umicom/designer/selection.h"
#include "umicom/designer/transaction_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_V2_CLIPBOARD_NODE_CAPACITY 64U

typedef struct UmiDesignerClipboardV2 {
    UmiDeclNode nodes[UMI_DESIGNER_V2_CLIPBOARD_NODE_CAPACITY];
    size_t node_count;
    uint64_t revision;
} UmiDesignerClipboardV2;

void umi_designer_clipboard_v2_init(UmiDesignerClipboardV2 *clipboard);
UmiStatus umi_designer_clipboard_v2_copy(UmiDesignerClipboardV2 *clipboard,
                                         const UmiDesignerDocument *document,
                                         const UmiDesignerSelection *selection);
UmiStatus umi_designer_clipboard_v2_paste(const UmiDesignerClipboardV2 *clipboard,
                                          UmiDesignerDocument *document,
                                          UmiDesignerTransactionHistoryV2 *history,
                                          const char *target_parent_id,
                                          const char *id_prefix);

#ifdef __cplusplus
}
#endif
#endif
