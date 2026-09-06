/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_checkpoint.h
 *
 * PURPOSE:
 *   Save and recover the last explicitly saved native workspace through the
 *   existing Data Server chunk store and portable workspace layout codec.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKSPACE_CHECKPOINT_H
#define UMICOM_UI_WORKSPACE_CHECKPOINT_H

#include "umicom/data/data_server.h"
#include "umicom/ui/workspace_customisation_persistence.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Scope identifiers are stable keys, not filesystem paths. Each application
 * and workspace ID accepts up to 63 ASCII letters, digits, dots, underscores
 * or hyphens. The owning layout prefix must end with a dot and a restored
 * layout must contain a nonempty suffix. Strings are borrowed for one call. */
typedef struct UmiUiWorkspaceCheckpointScope {
    const char *application_id;
    const char *workspace_id;
    const char *layout_prefix;
} UmiUiWorkspaceCheckpointScope;

/* Storage revision is independent of model/layout revisions. Recovery reports
 * the current primary's trustworthy CAS revision, not the older backup's.
 * If storage_revision_known is false, Save must remain unavailable until an
 * explicit repair restores trustworthy storage metadata. Memory stores and
 * SQLite :memory: connections never claim restart durability. */
typedef struct UmiUiWorkspaceCheckpointReport {
    char layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    uint64_t storage_revision;
    uint64_t saved_at_ns;
    bool storage_revision_known;
    bool durable;
    bool recovered_last_good;
    UmiStatus primary_status;
} UmiUiWorkspaceCheckpointReport;

/* Save only a committed, validated active layout. expected_storage_revision
 * is zero for a missing primary; otherwise it is the last observed storage
 * revision. A mismatch fails without replacing the primary or last-good copy.
 * Only a fully validated primary can become the last-good checkpoint.
 * The borrowed server must outlive the call; same-thread calls must not be
 * nested inside a caller-owned Data Server transaction. */
UmiStatus umi_ui_workspace_checkpoint_save(
    UmiDataServer *server, const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *model, uint64_t saved_at_ns,
    uint64_t expected_storage_revision, UmiUiWorkspaceCheckpointReport *out_report);

/* Load and validate text against the host's existing tool/context catalogue
 * without changing that model. Try the last-good checkpoint only when the
 * primary cannot be used. The output text remains unchanged on failure; the
 * report is cleared then filled with available diagnostics. A missing store
 * reports NOT_FOUND, storage_revision=0 and storage_revision_known=true.
 * This operation performs no persistent writes and creates no storage backend.
 * Hosts can build a candidate GUI from this text before publishing its model. */
UmiStatus umi_ui_workspace_checkpoint_load_validated(
    UmiDataServer *server, const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *validation_model,
    char *out_text, size_t capacity, UmiUiWorkspaceCheckpointReport *out_report);

/* Recover through the existing rollback-safe importer. A failed read, invalid
 * checkpoint or active edit leaves the entire live customisation unchanged.
 * Only the last explicitly saved active layout is restored, not every named
 * layout, document buffer, process, database connection or application state. */
UmiStatus umi_ui_workspace_checkpoint_restore(
    UmiDataServer *server, const UmiUiWorkspaceCheckpointScope *scope,
    UmiUiWorkspaceCustomisation *model, UmiUiWorkspaceCheckpointReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
