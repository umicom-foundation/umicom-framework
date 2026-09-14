/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_library_checkpoint.h
 * PURPOSE: Explicit ordered named-layout archives through the existing Data
 * Server chunk store, separate from the active-layout checkpoint namespace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKSPACE_LIBRARY_CHECKPOINT_H
#define UMICOM_UI_WORKSPACE_LIBRARY_CHECKPOINT_H
#include "umicom/ui/workspace_checkpoint.h"
/* Version-one archive ceiling, including its terminating NUL. The payload
 * fits the existing 128 x 3600-byte chunk-store envelope without widening it. */
#define UMI_UI_WORKSPACE_LIBRARY_ARCHIVE_CAPACITY (450U * 1024U + 1U)
#ifdef __cplusplus
extern "C" {
#endif

/* The nested checkpoint retains established CAS, durability and recovery
 * semantics. Its layout_id names the saved active layout. Count is populated
 * only after the entire ordered archive has passed validation. */
typedef struct UmiUiWorkspaceLibraryCheckpointReport {
    UmiUiWorkspaceCheckpointReport checkpoint;
    size_t layout_count;
} UmiUiWorkspaceLibraryCheckpointReport;

/* Explicitly save every named layout, its order and the active identifier.
 * All layouts must be committed/locked; editing returns BUSY. A stale storage
 * revision leaves primary and last-good unchanged. No toolkit, application
 * document, theme or catalogue definitions are archived. The established
 * chunk-store capacity also bounds the complete archive (oversize fails).
 * Calls borrow the server and scope, must use its owning thread and must not
 * run inside a caller-owned Data Server transaction. Owners serialize writes
 * on a shared server handle across these multi-call operations. */
UmiStatus umi_ui_workspace_library_checkpoint_save(
    UmiDataServer *server, const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *model, uint64_t saved_at_ns,
    uint64_t expected_storage_revision,
    UmiUiWorkspaceLibraryCheckpointReport *out_report);

/* Read a complete candidate without publishing over the validation owner.
 * The candidate replaces the named list exactly, retains current tool,
 * template and theme catalogues, and uses current context definitions only.
 * Reverse context membership is projected for the restored active layout;
 * unrelated memberships and surviving same-group roles are preserved.
 * Every restored layout revision exceeds every current and saved layout
 * revision, and the customisation revision advances exactly once. Overflow,
 * unknown tools/groups, malformed or foreign records leave output unchanged.
 * out_candidate must not overlap validation_model or out_report; callers
 * normally heap-allocate it and publish only after their native view accepts
 * it. This function performs no persistent writes. Reports remain diagnostic
 * on failure; early failures preserve the established NOT_FOUND/unknown CAS
 * convention. A returned NOT_FOUND means both archive manifests are absent:
 * missing chunks or unknown dependencies in an existing archive are errors,
 * not an empty store. An active edit returns BUSY. */
UmiStatus umi_ui_workspace_library_checkpoint_load_candidate(
    UmiDataServer *server, const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *validation_model,
    UmiUiWorkspaceCustomisation *out_candidate,
    UmiUiWorkspaceLibraryCheckpointReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
