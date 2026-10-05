/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/session_snapshot.h
 *
 * PURPOSE:
 *   Project session state into a bounded persistence-friendly snapshot without owning Data Server storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_SESSION_SNAPSHOT_H
#define UMICOM_APPLICATION_RUNTIME_SESSION_SNAPSHOT_H

#include "umicom/application/runtime/session.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application session snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationSessionSnapshot {
    uint32_t structure_size;
    char application_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char layout_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char active_panel_ids[UMI_APPLICATION_RUNTIME_MAX_PANELS]
                         [UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    size_t active_panel_count;
    bool layout_locked;
    uint64_t revision;
} UmiApplicationSessionSnapshot;

/**
 * Provide the application session snapshot capture operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_snapshot_capture(
    const UmiApplicationSession *session,
    UmiApplicationSessionSnapshot *out_snapshot);
/**
 * Provide the application session snapshot restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_snapshot_restore(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationSessionSnapshot *snapshot,
    UmiApplicationSession *out_session);

/* Encode/decode passive session data. The byte format validates bounded
 * identifiers, counts, duplicate panels and integrity before publication.
 * NULL output bytes with zero capacity measures size. Failed decode leaves
 * the destination unchanged. Keep source and destination storage separate. */
UmiStatus umi_application_session_snapshot_archive_encode(
    const UmiApplicationSessionSnapshot *snapshot, void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_application_session_snapshot_archive_decode(
    const void *bytes, size_t byte_count, UmiApplicationSessionSnapshot *out_snapshot);

/* Capture a live session or preview an archive against a product catalogue.
 * Catalogue memory must outlive every resulting session. The preview does not
 * open panels, run commands, create connections or access storage. */
UmiStatus umi_application_session_archive_capture(const UmiApplicationSession *session,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_application_session_archive_preview(const UmiApplicationExperienceDefinition *experience,
    const void *bytes, size_t byte_count, UmiApplicationSession *out_candidate);

/* Apply only if the live revision still equals the host's observed revision.
 * Failure leaves the session unchanged; success advances the local revision
 * once and never installs the saved counter as authority. Call from the owner
 * thread with external synchronization. This updates a session value only:
 * a GUI host must prepare its presentation before publishing a replacement. */
/* A locked live layout returns UMI_STATUS_PERMISSION_DENIED. Unlock through
 * the session API, then review again using the resulting local revision. */
UmiStatus umi_application_session_archive_apply(UmiApplicationSession *session,
    uint64_t expected_revision, const void *bytes, size_t byte_count);

#ifdef __cplusplus
}
#endif

#endif
