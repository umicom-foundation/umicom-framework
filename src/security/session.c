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
static size_t session_find(const UmiSessionRegistry *registry, const char *id)
{ size_t i; for (i = 0U; i < registry->count; ++i) if (strcmp(registry->sessions[i].session_id, id) == 0) return i; return SIZE_MAX; }
UmiStatus umi_session_registry_create(UmiSessionRegistry **out_registry)
{
    UmiSessionRegistry *registry; UmiStatus status;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; registry = (UmiSessionRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&registry->mutex); if (status != UMI_STATUS_OK) { free(registry); return status; }
    registry->sequence = 1U; *out_registry = registry; return UMI_STATUS_OK;
}
void umi_session_registry_destroy(UmiSessionRegistry *registry)
{ if (registry != NULL) { umi_mutex_destroy(registry->mutex); free(registry); } }
UmiStatus umi_session_registry_issue(UmiSessionRegistry *registry,
                                     const char *principal_id,
                                     uint64_t now_ns,
                                     uint64_t lifetime_ns,
                                     UmiSessionRecord *out_session)
{
    UmiSessionRecord *session; UmiStatus status;
    if (registry == NULL || principal_id == NULL || principal_id[0] == '\0' || out_session == NULL || lifetime_ns == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(principal_id) >= UMI_IDENTITY_ID_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_mutex_lock(registry->mutex); if (status != UMI_STATUS_OK) return status;
    if (registry->count >= UMI_SESSION_REGISTRY_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    session = &registry->sessions[registry->count++];
    (void)snprintf(session->session_id, sizeof(session->session_id), "session-%016llx", (unsigned long long)registry->sequence++);
    (void)snprintf(session->principal_id, sizeof(session->principal_id), "%s", principal_id);
    session->issued_at_ns = now_ns; session->expires_at_ns = now_ns + lifetime_ns; session->last_seen_ns = now_ns; session->active = 1;
    *out_session = *session; (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
UmiStatus umi_session_registry_validate(UmiSessionRegistry *registry,
                                        const char *session_id,
                                        uint64_t now_ns,
                                        UmiSessionRecord *out_session)
{
    size_t index; UmiStatus status;
    if (registry == NULL || session_id == NULL || out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); if (status != UMI_STATUS_OK) return status;
    index = session_find(registry, session_id);
    if (index == SIZE_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND; }
    if (!registry->sessions[index].active || now_ns >= registry->sessions[index].expires_at_ns) {
        registry->sessions[index].active = 0; (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_PERMISSION_DENIED;
    }
    registry->sessions[index].last_seen_ns = now_ns; *out_session = registry->sessions[index];
    (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
UmiStatus umi_session_registry_revoke(UmiSessionRegistry *registry, const char *session_id)
{
    size_t index; UmiStatus status;
    if (registry == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); if (status != UMI_STATUS_OK) return status;
    index = session_find(registry, session_id); if (index == SIZE_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND; }
    registry->sessions[index].active = 0; (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
size_t umi_session_registry_cleanup(UmiSessionRegistry *registry, uint64_t now_ns)
{
    size_t read_index, write_index = 0U, removed = 0U;
    if (registry == NULL || umi_mutex_lock(registry->mutex) != UMI_STATUS_OK) return 0U;
    for (read_index = 0U; read_index < registry->count; ++read_index) {
        UmiSessionRecord *session = &registry->sessions[read_index];
        if (!session->active || now_ns >= session->expires_at_ns) { ++removed; continue; }
        if (write_index != read_index) registry->sessions[write_index] = *session;
        ++write_index;
    }
    registry->count = write_index;
    (void)umi_mutex_unlock(registry->mutex); return removed;
}
size_t umi_session_registry_count(const UmiSessionRegistry *registry)
{ size_t n = 0U; if (registry != NULL && umi_mutex_lock(registry->mutex) == UMI_STATUS_OK) { n = registry->count; (void)umi_mutex_unlock(registry->mutex); } return n; }
UmiStatus umi_session_registry_at(const UmiSessionRegistry *registry, size_t index, UmiSessionRecord *out_session)
{
    UmiStatus status; if (registry == NULL || out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); if (status != UMI_STATUS_OK) return status;
    if (index >= registry->count) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND; }
    *out_session = registry->sessions[index]; (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
