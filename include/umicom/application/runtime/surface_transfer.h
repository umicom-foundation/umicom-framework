/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/surface_transfer.h
 *
 * PURPOSE:
 *   Define the small, portable acknowledgement protocol used when an
 *   application surface moves between Framework Workbench Hosts.  The token
 *   carries references to session state; it never carries widget pointers,
 *   credentials or private service objects.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_RUNTIME_SURFACE_TRANSFER_H
#define UMICOM_APPLICATION_RUNTIME_SURFACE_TRANSFER_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Keep transfer records bounded so a host can validate one without allocation. */
#define UMI_APPLICATION_SURFACE_TRANSFER_API_VERSION 1U

/**
 * List the lifecycle states accepted by the application-surface transfer
 * protocol.  A source releases its tab only after the token reaches ACCEPTED.
 */
typedef enum UmiApplicationSurfaceTransferState {
    UMI_APPLICATION_SURFACE_TRANSFER_PENDING = 1,
    UMI_APPLICATION_SURFACE_TRANSFER_ACCEPTED = 2,
    UMI_APPLICATION_SURFACE_TRANSFER_COMMITTED = 3,
    UMI_APPLICATION_SURFACE_TRANSFER_CANCELLED = 4,
    UMI_APPLICATION_SURFACE_TRANSFER_EXPIRED = 5
} UmiApplicationSurfaceTransferState;

/**
 * Represent an acknowledged application-surface transfer token.  The
 * checkpoint and capability fields are opaque references or fingerprints;
 * callers keep the actual session data in their own Framework services.
 */
typedef struct UmiApplicationSurfaceTransferToken {
    uint32_t structure_size;
    uint32_t api_version;
    char token_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char application_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char session_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char source_host_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char destination_host_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char checkpoint_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char capability_fingerprint[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    uint64_t issued_at_ms;
    uint64_t expires_at_ms;
    UmiApplicationSurfaceTransferState state;
    uint64_t revision;
} UmiApplicationSurfaceTransferToken;

/**
 * Create a short-lived pending token from stable product and session
 * references.  The caller supplies an opaque token ID from its secure token
 * source; this function does not invent predictable security credentials.
 */
UmiStatus umi_application_surface_transfer_issue(
    const char *token_id,
    const char *application_id,
    const char *session_id,
    const char *source_host_id,
    const char *checkpoint_id,
    const char *capability_fingerprint,
    uint64_t issued_at_ms,
    uint64_t lifetime_ms,
    UmiApplicationSurfaceTransferToken *out_token);

/**
 * Validate token shape, lifecycle fields and expiry without changing the
 * token.  A pending or accepted token past its deadline returns TIMEOUT.
 */
UmiStatus umi_application_surface_transfer_validate(
    const UmiApplicationSurfaceTransferToken *token,
    uint64_t now_ms);

/**
 * Acknowledge that a destination host rehydrated the referenced session.
 * Repeating the same acknowledgement is safe; a different destination is
 * rejected so one token cannot acquire two owners.
 */
UmiStatus umi_application_surface_transfer_acknowledge(
    UmiApplicationSurfaceTransferToken *token,
    const char *destination_host_id,
    uint64_t now_ms);

/**
 * Commit source ownership release after destination acknowledgement.  This
 * operation is idempotent for an already committed token and never serialises
 * the session contents itself.
 */
UmiStatus umi_application_surface_transfer_commit(
    UmiApplicationSurfaceTransferToken *token,
    uint64_t now_ms);

/**
 * Cancel a pending or accepted transfer so the source keeps its usable tab.
 * Cancellation is idempotent and cannot undo a committed ownership release.
 */
UmiStatus umi_application_surface_transfer_cancel(
    UmiApplicationSurfaceTransferToken *token,
    uint64_t now_ms);

/** Return stable text for menus, diagnostics and generated documentation. */
const char *umi_application_surface_transfer_state_text(
    UmiApplicationSurfaceTransferState state);

#ifdef __cplusplus
}
#endif

#endif
