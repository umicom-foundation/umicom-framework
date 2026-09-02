/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/session.c
 *
 * PURPOSE:
 *   Implement deterministic, revocable sessions with expiry checks and last-seen
 *   tracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/session.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
struct UmiSessionRegistry { UmiSessionRecord sessions[UMI_SESSION_REGISTRY_MAX]; size_t count; uint64_t sequence; UmiMutex *mutex; };
/* Find session while leaving the underlying catalogue or model owned by this module. */
static size_t session_find(const UmiSessionRegistry *registry, const char *id)
{ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for (i = 0U; i < registry->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->sessions[i].session_id, id) == 0) return i; return SIZE_MAX; }
/*
 * Initialise session registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_session_registry_create(UmiSessionRegistry **out_registry)
{
    UmiSessionRegistry *registry; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; registry = (UmiSessionRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { free(registry); return status; }
    registry->sequence = 1U; *out_registry = registry; return UMI_STATUS_OK;
}
/*
 * Release or reset state held by session registry so the same storage can be reused
 * safely.
 */
void umi_session_registry_destroy(UmiSessionRegistry *registry)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL) { umi_mutex_destroy(registry->mutex); free(registry); } }
/*
 * Provide the session registry issue operation used by this module and its client
 * applications.
 */
UmiStatus umi_session_registry_issue(UmiSessionRegistry *registry,
                                     const char *principal_id,
                                     uint64_t now_ns,
                                     uint64_t lifetime_ns,
                                     UmiSessionRecord *out_session)
{
    UmiSessionRecord *session; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || principal_id == NULL || principal_id[0] == '\0' || out_session == NULL || lifetime_ns == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(principal_id) >= UMI_IDENTITY_ID_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_mutex_lock(registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_SESSION_REGISTRY_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    session = &registry->sessions[registry->count++];
    (void)snprintf(session->session_id, sizeof(session->session_id), "session-%016llx", (unsigned long long)registry->sequence++);
    (void)snprintf(session->principal_id, sizeof(session->principal_id), "%s", principal_id);
    session->issued_at_ns = now_ns; session->expires_at_ns = now_ns + lifetime_ns; session->last_seen_ns = now_ns; session->active = 1;
    *out_session = *session; (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
/* Check that session registry satisfies its contract before another service relies on it. */
UmiStatus umi_session_registry_validate(UmiSessionRegistry *registry,
                                        const char *session_id,
                                        uint64_t now_ns,
                                        UmiSessionRecord *out_session)
{
    size_t index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || session_id == NULL || out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    index = session_find(registry, session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!registry->sessions[index].active || now_ns >= registry->sessions[index].expires_at_ns) {
        registry->sessions[index].active = 0; (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_PERMISSION_DENIED;
    }
    registry->sessions[index].last_seen_ns = now_ns; *out_session = registry->sessions[index];
    (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
/*
 * Provide the session registry revoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_session_registry_revoke(UmiSessionRegistry *registry, const char *session_id)
{
    size_t index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    index = session_find(registry, session_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND; }
    registry->sessions[index].active = 0; (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
/*
 * Provide the session registry cleanup operation used by this module and its client
 * applications.
 */
size_t umi_session_registry_cleanup(UmiSessionRegistry *registry, uint64_t now_ns)
{
    size_t read_index, write_index = 0U, removed = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || umi_mutex_lock(registry->mutex) != UMI_STATUS_OK) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (read_index = 0U; read_index < registry->count; ++read_index) {
        UmiSessionRecord *session = &registry->sessions[read_index];
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!session->active || now_ns >= session->expires_at_ns) { ++removed; continue; }
        /* Apply this branch only when its contract condition is satisfied. */
        if (write_index != read_index) registry->sessions[write_index] = *session;
        ++write_index;
    }
    registry->count = write_index;
    (void)umi_mutex_unlock(registry->mutex); return removed;
}
/*
 * Return the number of records represented by session registry without changing their
 * state.
 */
size_t umi_session_registry_count(const UmiSessionRegistry *registry)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL && umi_mutex_lock(registry->mutex) == UMI_STATUS_OK) { n = registry->count; (void)umi_mutex_unlock(registry->mutex); } return n; }
/*
 * Find session registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_session_registry_at(const UmiSessionRegistry *registry, size_t index, UmiSessionRecord *out_session)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry == NULL || out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND; }
    *out_session = registry->sessions[index]; (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
