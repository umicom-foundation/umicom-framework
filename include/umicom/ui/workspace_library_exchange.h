/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_library_exchange.h
 * PURPOSE: Exchange complete named-layout libraries with an immutable import review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKSPACE_LIBRARY_EXCHANGE_H
#define UMICOM_UI_WORKSPACE_LIBRARY_EXCHANGE_H
#include "umicom/ui/workspace_library_checkpoint.h"
#ifdef __cplusplus
extern "C" {
#endif

/* A review owns the exact input bytes and a copied summary. It contains no
 * GTK objects, file handles, storage connection or authority to execute work.
 * Keep it on the workspace owner's thread and destroy it when dismissed. */
typedef struct UmiUiWorkspaceLibraryImport UmiUiWorkspaceLibraryImport;

/* Export the committed library using the same grammar as library checkpoints.
 * out_size excludes the terminating NUL; capacity must include that byte.
 * NULL bytes with zero capacity measures the required length. A short buffer
 * returns CAPACITY_EXCEEDED and the required length without writing bytes.
 * Other errors leave both outputs unchanged. Outputs must not overlap inputs
 * or one another. Export does not save storage or change the live workspace. */
UmiStatus umi_ui_workspace_library_export(
    const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *model, uint64_t saved_at_ns,
    char *bytes, size_t capacity, size_t *out_size);

/* Copy and validate a bounded byte span before publishing a review. size must
 * exclude any NUL terminator. Embedded NUL, trailing data, unknown tools,
 * foreign layout namespaces and editing workspaces are refused. Output is
 * unchanged on failure. Scope strings are copied rather than borrowed.
 * The summary describes list changes; it is not a geometry diff. */
UmiStatus umi_ui_workspace_library_import_review(
    const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *model, const void *bytes, size_t size,
    UmiUiWorkspaceLibraryImport **out_review);

/* Read the saved library once, including last-good recovery when needed,
 * into the same immutable review used for file imports. Inspect its complete
 * current/saved geometry and report before calling import_candidate. Apply
 * never rereads storage: later writes cannot substitute another arrangement.
 * The original model must remain alive and must not be reinitialised until
 * the review is destroyed. Another model is refused even at the same revision.
 * A stored review survives storage rebinding/destruction because it owns its
 * bytes; it grants no permission to overwrite the store. Hosts must retain
 * their existing Save revision and handle conflicts explicitly. Ordinary
 * checkpoint_preview and confirmed reread Restore retain their original APIs.
 * Failure leaves the output pointer and live model unchanged. Owner-thread,
 * no-active-transaction restrictions of checkpoint_load_candidate apply. */
UmiStatus umi_ui_workspace_library_checkpoint_review(UmiDataServer *server,
    const UmiUiWorkspaceCheckpointScope *scope, const UmiUiWorkspaceCustomisation *model,
    UmiUiWorkspaceLibraryImport **out_review);

/* Borrow immutable summary evidence only while the review remains alive. */
const UmiUiWorkspaceLibraryPreview *umi_ui_workspace_library_import_summary(
    const UmiUiWorkspaceLibraryImport *review);

/* Full geometry evidence supplements the summary. imported=false selects the
 * current saved library; true selects the imported library. Rows preserve list
 * order and contain complete copied panel data, including hidden panels and
 * inactive layouts. NULL review gives count zero; an invalid index gives NULL.
 * Borrowed rows are immutable and live until import_destroy. Revision counters
 * are bookkeeping, not user-visible layout changes. Use layout/window IDs to
 * match rows, rather than addresses, array positions or display titles. */
size_t umi_ui_workspace_library_import_layout_count(
    const UmiUiWorkspaceLibraryImport *review, bool imported);
const UmiUiWorkspaceLayout *umi_ui_workspace_library_import_layout(
    const UmiUiWorkspaceLibraryImport *review, bool imported, size_t index);

/* Revalidate the frozen bytes against the same scope and current revision.
 * A changed workspace must be reviewed again. The result is a private
 * candidate; a native host publishes it only after its widgets accept the new
 * layout. All failures preserve out_candidate. No file is reread and no
 * document, process, trade or account operation is restored or executed. */
UmiStatus umi_ui_workspace_library_import_candidate(
    const UmiUiWorkspaceLibraryImport *review,
    const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *model,
    UmiUiWorkspaceCustomisation *out_candidate);
void umi_ui_workspace_library_import_destroy(UmiUiWorkspaceLibraryImport *review);

#ifdef __cplusplus
}
#endif
#endif
