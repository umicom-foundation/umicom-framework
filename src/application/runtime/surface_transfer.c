/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/surface_transfer.c
 *
 * PURPOSE:
 *   Implement the bounded acknowledgement protocol used to transfer an
 *   application surface between Framework Workbench Hosts.  The protocol
 *   protects ownership transitions while leaving session serialisation and
 *   native window creation to the appropriate Framework services.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/surface_transfer.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Copy a stable identifier without silently changing the token's meaning. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;

    /* All protocol identities are required and must fit their fixed fields. */
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    /* A truncated ID could point at a different session, so reject it. */
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Confirm that a fixed-size text field contains a terminated non-empty value. */
static int text_is_valid(const char *value, size_t capacity, int required)
{
    /* memchr avoids reading beyond a malformed fixed-size record. */
    if (value == NULL || capacity == 0U ||
        memchr(value, '\0', capacity) == NULL) {
        return 0;
    }
    return required == 0 || value[0] != '\0';
}

/* Advance the revision while refusing an observable unsigned wraparound. */
static UmiStatus advance_revision(UmiApplicationSurfaceTransferToken *token)
{
    /* A wrapped revision would make an old acknowledgement look current. */
    if (token == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (token->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    token->revision += 1U;
    return UMI_STATUS_OK;
}

/* Mark an uncommitted token expired when its caller's clock passes the deadline. */
static UmiStatus expire_if_needed(
    UmiApplicationSurfaceTransferToken *token,
    uint64_t now_ms)
{
    UmiStatus status;

    /* Only pending and accepted transfers can become expired. */
    if (token == NULL ||
        (token->state != UMI_APPLICATION_SURFACE_TRANSFER_PENDING &&
         token->state != UMI_APPLICATION_SURFACE_TRANSFER_ACCEPTED)) {
        return UMI_STATUS_OK;
    }
    if (now_ms <= token->expires_at_ms) return UMI_STATUS_OK;
    /* Check the revision before changing state so expiry is all-or-nothing. */
    if (token->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    token->state = UMI_APPLICATION_SURFACE_TRANSFER_EXPIRED;
    status = advance_revision(token);
    /* The overflow check above guarantees this cannot fail after the mutation. */
    return status;
}

/* Check all fields that must remain stable while a token crosses host windows. */
static UmiStatus validate_shape(
    const UmiApplicationSurfaceTransferToken *token)
{
    /* The structure size and API version make incompatible records explicit. */
    if (token == NULL ||
        token->structure_size != sizeof(*token) ||
        token->api_version != UMI_APPLICATION_SURFACE_TRANSFER_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!text_is_valid(token->token_id,
                       sizeof(token->token_id), 1) ||
        !text_is_valid(token->application_id,
                       sizeof(token->application_id), 1) ||
        !text_is_valid(token->session_id,
                       sizeof(token->session_id), 1) ||
        !text_is_valid(token->source_host_id,
                       sizeof(token->source_host_id), 1) ||
        !text_is_valid(token->checkpoint_id,
                       sizeof(token->checkpoint_id), 1) ||
        !text_is_valid(token->capability_fingerprint,
                       sizeof(token->capability_fingerprint), 1)) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* A zero-length interval or wrapped deadline is never a usable transfer. */
    if (token->expires_at_ms <= token->issued_at_ms || token->revision == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (token->state < UMI_APPLICATION_SURFACE_TRANSFER_PENDING ||
        token->state > UMI_APPLICATION_SURFACE_TRANSFER_EXPIRED) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Accepted and committed records must identify their acknowledged owner. */
    if ((token->state == UMI_APPLICATION_SURFACE_TRANSFER_ACCEPTED ||
         token->state == UMI_APPLICATION_SURFACE_TRANSFER_COMMITTED) &&
        !text_is_valid(token->destination_host_id,
                       sizeof(token->destination_host_id), 1)) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Pending records cannot already claim a destination. */
    if (token->state == UMI_APPLICATION_SURFACE_TRANSFER_PENDING &&
        token->destination_host_id[0] != '\0') {
        return UMI_STATUS_INVALID_STATE;
    }
    /* A cancelled or expired token may retain an acknowledgement for auditing. */
    if (!text_is_valid(token->destination_host_id,
                       sizeof(token->destination_host_id), 0)) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* Create a pending transfer record from caller-owned stable references. */
UmiStatus umi_application_surface_transfer_issue(
    const char *token_id,
    const char *application_id,
    const char *session_id,
    const char *source_host_id,
    const char *checkpoint_id,
    const char *capability_fingerprint,
    uint64_t issued_at_ms,
    uint64_t lifetime_ms,
    UmiApplicationSurfaceTransferToken *out_token)
{
    UmiApplicationSurfaceTransferToken candidate;
    UmiStatus status;

    /* A caller must provide every reference and a finite positive lifetime. */
    if (out_token == NULL || token_id == NULL || application_id == NULL ||
        session_id == NULL || source_host_id == NULL || checkpoint_id == NULL ||
        capability_fingerprint == NULL || lifetime_ms == 0U ||
        issued_at_ms > UINT64_MAX - lifetime_ms) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&candidate, 0, sizeof(candidate));
    candidate.structure_size = sizeof(candidate);
    candidate.api_version = UMI_APPLICATION_SURFACE_TRANSFER_API_VERSION;
    status = copy_text(candidate.token_id, sizeof(candidate.token_id), token_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(candidate.application_id,
                           sizeof(candidate.application_id), application_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(candidate.session_id,
                           sizeof(candidate.session_id), session_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(candidate.source_host_id,
                           sizeof(candidate.source_host_id), source_host_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(candidate.checkpoint_id,
                           sizeof(candidate.checkpoint_id), checkpoint_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(candidate.capability_fingerprint,
                           sizeof(candidate.capability_fingerprint),
                           capability_fingerprint);
    }
    /* Publish a complete record only after every input copied successfully. */
    if (status != UMI_STATUS_OK) return status;
    candidate.issued_at_ms = issued_at_ms;
    candidate.expires_at_ms = issued_at_ms + lifetime_ms;
    candidate.state = UMI_APPLICATION_SURFACE_TRANSFER_PENDING;
    candidate.revision = 1U;
    *out_token = candidate;
    return UMI_STATUS_OK;
}

/* Validate a token without changing it, including its caller-supplied clock view. */
UmiStatus umi_application_surface_transfer_validate(
    const UmiApplicationSurfaceTransferToken *token,
    uint64_t now_ms)
{
    UmiStatus status = validate_shape(token);

    /* Shape errors must be reported before time comparisons. */
    if (status != UMI_STATUS_OK) return status;
    if ((token->state == UMI_APPLICATION_SURFACE_TRANSFER_PENDING ||
         token->state == UMI_APPLICATION_SURFACE_TRANSFER_ACCEPTED) &&
        now_ms > token->expires_at_ms) {
        return UMI_STATUS_TIMEOUT;
    }
    return UMI_STATUS_OK;
}

/* Record destination ownership acknowledgement while keeping duplicate requests safe. */
UmiStatus umi_application_surface_transfer_acknowledge(
    UmiApplicationSurfaceTransferToken *token,
    const char *destination_host_id,
    uint64_t now_ms)
{
    UmiStatus status;
    char destination_copy[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];

    /* A destination must be named before it can own a rehydrated session. */
    if (token == NULL || destination_host_id == NULL ||
        destination_host_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = validate_shape(token);
    if (status != UMI_STATUS_OK) return status;
    status = expire_if_needed(token, now_ms);
    if (status != UMI_STATUS_OK) return status;
    if (token->state == UMI_APPLICATION_SURFACE_TRANSFER_EXPIRED) {
        return UMI_STATUS_TIMEOUT;
    }
    if (token->state == UMI_APPLICATION_SURFACE_TRANSFER_CANCELLED) {
        return UMI_STATUS_CANCELLED;
    }
    /* A duplicate acknowledgement from the same host is deliberately idempotent. */
    if (token->state == UMI_APPLICATION_SURFACE_TRANSFER_ACCEPTED ||
        token->state == UMI_APPLICATION_SURFACE_TRANSFER_COMMITTED) {
        return strcmp(token->destination_host_id, destination_host_id) == 0
            ? UMI_STATUS_OK
            : UMI_STATUS_ALREADY_EXISTS;
    }
    if (token->state != UMI_APPLICATION_SURFACE_TRANSFER_PENDING) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Copy into temporary storage before changing the token, preserving an
     * unchanged pending record when either validation or revision publication
     * fails. */
    status = copy_text(destination_copy,
                       sizeof(destination_copy),
                       destination_host_id);
    if (status != UMI_STATUS_OK) return status;
    if (token->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = advance_revision(token);
    if (status != UMI_STATUS_OK) return status;
    (void)memcpy(token->destination_host_id,
                 destination_copy,
                 sizeof(token->destination_host_id));
    token->state = UMI_APPLICATION_SURFACE_TRANSFER_ACCEPTED;
    return UMI_STATUS_OK;
}

/* Release source ownership only after an acknowledged destination is recorded. */
UmiStatus umi_application_surface_transfer_commit(
    UmiApplicationSurfaceTransferToken *token,
    uint64_t now_ms)
{
    UmiStatus status;

    /* A null record cannot prove that another host accepted the session. */
    if (token == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_shape(token);
    if (status != UMI_STATUS_OK) return status;
    status = expire_if_needed(token, now_ms);
    if (status != UMI_STATUS_OK) return status;
    if (token->state == UMI_APPLICATION_SURFACE_TRANSFER_COMMITTED) {
        return UMI_STATUS_OK;
    }
    if (token->state == UMI_APPLICATION_SURFACE_TRANSFER_EXPIRED) {
        return UMI_STATUS_TIMEOUT;
    }
    if (token->state == UMI_APPLICATION_SURFACE_TRANSFER_CANCELLED) {
        return UMI_STATUS_CANCELLED;
    }
    /* Pending means the source still owns its tab and must not release it. */
    if (token->state != UMI_APPLICATION_SURFACE_TRANSFER_ACCEPTED) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (token->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    token->state = UMI_APPLICATION_SURFACE_TRANSFER_COMMITTED;
    status = advance_revision(token);
    /* The overflow check above guarantees this cannot fail after the mutation. */
    return status;
}

/* Cancel an uncommitted transfer while leaving the source session available. */
UmiStatus umi_application_surface_transfer_cancel(
    UmiApplicationSurfaceTransferToken *token,
    uint64_t now_ms)
{
    UmiStatus status;

    /* Cancellation is allowed only while source ownership is still recoverable. */
    if (token == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_shape(token);
    if (status != UMI_STATUS_OK) return status;
    if (token->state == UMI_APPLICATION_SURFACE_TRANSFER_CANCELLED) {
        return UMI_STATUS_OK;
    }
    if (token->state == UMI_APPLICATION_SURFACE_TRANSFER_COMMITTED) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = expire_if_needed(token, now_ms);
    if (status != UMI_STATUS_OK) return status;
    if (token->state == UMI_APPLICATION_SURFACE_TRANSFER_EXPIRED) {
        return UMI_STATUS_TIMEOUT;
    }
    if (token->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    token->state = UMI_APPLICATION_SURFACE_TRANSFER_CANCELLED;
    status = advance_revision(token);
    /* The overflow check above guarantees this cannot fail after the mutation. */
    return status;
}

/* Return stable state text for diagnostics, menus and generated documentation. */
const char *umi_application_surface_transfer_state_text(
    UmiApplicationSurfaceTransferState state)
{
    /* Keep unknown values visible rather than presenting an empty status. */
    switch (state) {
    case UMI_APPLICATION_SURFACE_TRANSFER_PENDING: return "pending";
    case UMI_APPLICATION_SURFACE_TRANSFER_ACCEPTED: return "accepted";
    case UMI_APPLICATION_SURFACE_TRANSFER_COMMITTED: return "committed";
    case UMI_APPLICATION_SURFACE_TRANSFER_CANCELLED: return "cancelled";
    case UMI_APPLICATION_SURFACE_TRANSFER_EXPIRED: return "expired";
    default: return "unknown";
    }
}
