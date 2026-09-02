/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/clipboard.h
 *
 * PURPOSE:
 *   Copy semantic subtrees without toolkit objects and paste them with
 *   collision-free identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_CLIPBOARD_H
#define UMICOM_WORKBENCH_DESIGNER_CLIPBOARD_H

#include "umicom/workbench_designer/selection.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer clipboard data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerClipboard {
    UmiWorkbenchLayoutNode nodes[UMI_WORKBENCH_DESIGNER_MAX_CLIPBOARD_NODES];
    size_t node_count;
    size_t root_indices[UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS];
    size_t root_count;
    bool cut;
    char source_layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint64_t source_revision;
    uint64_t revision;
} UmiWorkbenchDesignerClipboard;

/**
 * Initialise workbench designer clipboard from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_clipboard_init(UmiWorkbenchDesignerClipboard *clipboard);
/**
 * Release or reset state held by workbench designer clipboard so the same storage can be
 * reused safely.
 */
void umi_workbench_designer_clipboard_clear(UmiWorkbenchDesignerClipboard *clipboard);
/**
 * Copy workbench designer clipboard into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_workbench_designer_clipboard_copy(UmiWorkbenchDesignerClipboard *clipboard, const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerSelection *selection, bool cut);
/**
 * Provide the workbench designer clipboard paste operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_clipboard_paste(UmiWorkbenchDesignerClipboard *clipboard, UmiWorkbenchLayoutDocument *document, const char *parent_node_id, size_t position, const char *identifier_suffix, UmiWorkbenchDesignerSelection *out_selection);
/**
 * Provide the workbench designer clipboard empty operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_clipboard_empty(const UmiWorkbenchDesignerClipboard *clipboard);

#ifdef __cplusplus
}
#endif

#endif
