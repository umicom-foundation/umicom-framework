/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/types.c
 *
 * PURPOSE:
 *   Implement text conversion, bounded copying, stable hashing and common value helpers for the layout persistence platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/types.h"
#include <string.h>



#define UMI_WORKBENCH_LAYOUT_DATA_FNV_OFFSET UINT64_C(1469598103934665603)
#define UMI_WORKBENCH_LAYOUT_DATA_FNV_PRIME UINT64_C(1099511628211)

#define UMI_TEXT_CASE(value, text) case value: return text

/*
 * Provide the workbench layout data record kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_record_kind_text(
    UmiWorkbenchLayoutDataRecordKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
                  "layout-manifest");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK,
                  "layout-chunk");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST,
                  "session-manifest");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK,
                  "session-chunk");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION, "revision");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_GRANT, "grant");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_LEASE, "lease");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_CHANGE, "change");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX, "outbox");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_CONFLICT, "conflict");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_SYNC_CURSOR,
                  "sync-cursor");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_OFFLINE_OPERATION,
                  "offline-operation");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_PRESENCE, "presence");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_MIGRATION, "migration");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_RECORD_METADATA, "metadata");
    default: return "invalid";
    }
}

/*
 * Provide the workbench layout data change kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_change_kind_text(
    UmiWorkbenchLayoutDataChangeKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED, "created");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED, "updated");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CHANGE_DELETED, "deleted");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CHANGE_SHARED, "shared");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UNSHARED, "unshared");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CHANGE_LOCKED, "locked");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UNLOCKED, "unlocked");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CHANGE_SESSION_CHECKPOINTED,
                  "session-checkpointed");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CONFLICT_RECORDED,
                  "conflict-recorded");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CONFLICT_RESOLVED,
                  "conflict-resolved");
    default: return "invalid";
    }
}

/*
 * Provide the workbench layout data permission text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_permission_text(
    UmiWorkbenchLayoutDataPermission permission)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (permission) {
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW, "view");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_EDIT, "edit");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_SHARE, "share");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_DELETE, "delete");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_LOCK, "lock");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_ADMINISTER,
                  "administer");
    default: return "invalid";
    }
}

/*
 * Provide the workbench layout data lease state text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_lease_state_text(
    UmiWorkbenchLayoutDataLeaseState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_LEASE_AVAILABLE, "available");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_LEASE_HELD, "held");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_LEASE_EXPIRED, "expired");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_LEASE_RELEASED, "released");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_LEASE_REVOKED, "revoked");
    default: return "invalid";
    }
}

/*
 * Provide the workbench layout data outbox state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_outbox_state_text(
    UmiWorkbenchLayoutDataOutboxState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_PENDING, "pending");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_LEASED, "leased");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DELIVERED, "delivered");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_RETRY_WAIT, "retry-wait");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DEAD_LETTER,
                  "dead-letter");
    default: return "invalid";
    }
}

/*
 * Provide the workbench layout data conflict kind text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_conflict_kind_text(
    UmiWorkbenchLayoutDataConflictKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_CONCURRENT_UPDATE,
                  "concurrent-update");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_DELETE_VERSUS_UPDATE,
                  "delete-versus-update");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_PERMISSION,
                  "permission");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_SCHEMA, "schema");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_INTEGRITY,
                  "integrity");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_MISSING_BASE,
                  "missing-base");
    default: return "invalid";
    }
}

/*
 * Provide the workbench layout data conflict state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_conflict_state_text(
    UmiWorkbenchLayoutDataConflictState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_OPEN, "open");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_LOCAL,
                  "resolved-local");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_REMOTE,
                  "resolved-remote");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_MERGED,
                  "resolved-merged");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_DISMISSED, "dismissed");
    default: return "invalid";
    }
}

/*
 * Provide the workbench layout data sync action text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_sync_action_text(
    UmiWorkbenchLayoutDataSyncAction action)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_SYNC_NO_ACTION, "no-action");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_LOCAL,
                  "create-local");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_LOCAL,
                  "update-local");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_LOCAL,
                  "delete-local");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_REMOTE,
                  "create-remote");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_REMOTE,
                  "update-remote");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_REMOTE,
                  "delete-remote");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_SYNC_RECORD_CONFLICT,
                  "record-conflict");
    default: return "invalid";
    }
}

/*
 * Provide the workbench layout data operation state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_operation_state_text(
    UmiWorkbenchLayoutDataOperationState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_OPERATION_PENDING, "pending");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_OPERATION_CLAIMED, "claimed");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_OPERATION_COMPLETED, "completed");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_OPERATION_RETRY_WAIT,
                  "retry-wait");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_OPERATION_FAILED, "failed");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_OPERATION_CANCELLED,
                  "cancelled");
    default: return "invalid";
    }
}

/*
 * Provide the workbench layout data health state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_health_state_text(
    UmiWorkbenchLayoutDataHealthState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_HEALTH_HEALTHY, "healthy");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_HEALTH_DEGRADED, "degraded");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_HEALTH_UNHEALTHY, "unhealthy");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_HEALTH_UNAVAILABLE,
                  "unavailable");
    default: return "invalid";
    }
}

/*
 * Provide the workbench layout data controller state text operation used by this module
 * and its client applications.
 */
const char *umi_workbench_layout_data_controller_state_text(
    UmiWorkbenchLayoutDataControllerState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_CREATED, "created");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_INITIALISED,
                  "initialised");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_RUNNING, "running");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_QUIESCED,
                  "quiesced");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_STOPPED, "stopped");
    UMI_TEXT_CASE(UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_FAILED, "failed");
    default: return "invalid";
    }
}

/*
 * Provide the workbench layout data hash bytes operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_layout_data_hash_bytes(
    const void *data,
    size_t size)
{
    const unsigned char *bytes = (const unsigned char *)data;
    uint64_t hash = UMI_WORKBENCH_LAYOUT_DATA_FNV_OFFSET;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (data == NULL && size != 0U) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < size; ++index) {
        hash ^= (uint64_t)bytes[index];
        hash *= UMI_WORKBENCH_LAYOUT_DATA_FNV_PRIME;
    }
    return hash;
}

/*
 * Provide the workbench layout data hash text operation used by this module and its client
 * applications.
 */
uint64_t umi_workbench_layout_data_hash_text(
    const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    return umi_workbench_layout_data_hash_bytes(text, strlen(text));
}

/*
 * Provide the workbench layout data copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_data_copy_text(
    char *destination,
    size_t capacity,
    const char *source,
    bool allow_empty)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!allow_empty && source[0] == '\0') {
        destination[0] = '\0';
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout data text equal operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_data_text_equal(
    const char *left,
    const char *right)
{
    return left != NULL && right != NULL && strcmp(left, right) == 0;
}

/*
 * Provide the workbench layout data text has prefix operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_data_text_has_prefix(
    const char *text,
    const char *prefix)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || prefix == NULL) return false;
    length = strlen(prefix);
    return strncmp(text, prefix, length) == 0;
}

/*
 * Provide the workbench layout data time expired operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_data_time_expired(
    uint64_t expires_at_ms,
    uint64_t now_ms)
{
    return expires_at_ms != 0U && expires_at_ms <= now_ms;
}
