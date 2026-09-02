/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/types.h
 *
 * PURPOSE:
 *   Define stable records, state machines and bounded value types for Data Server-backed layout persistence, sharing, synchronisation and recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_TYPES_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_LAYOUT_DATA_SCHEMA_VERSION 1U
#define UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY 192U
#define UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY 3900U
#define UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY 256U
#define UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY 512U
#define UMI_WORKBENCH_LAYOUT_DATA_PATH_CAPACITY 768U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_CHUNKS 128U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_REVISIONS 256U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_GRANTS 128U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_LEASES 128U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_CHANGES 512U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_OUTBOX 256U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_CONFLICTS 128U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_SYNC_ITEMS 256U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_OFFLINE_OPERATIONS 256U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_PRESENCE 128U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_MIGRATIONS 64U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_RECONCILIATION_ISSUES 256U
#define UMI_WORKBENCH_LAYOUT_DATA_MAX_BACKUP_RECORDS 2048U
#define UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE ((size_t)-1)

/**
 * List the named workbench layout data record kind values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataRecordKind {
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST = 1,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK = 2,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST = 3,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK = 4,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION = 5,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_GRANT = 6,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_LEASE = 7,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_CHANGE = 8,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX = 9,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_CONFLICT = 10,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_SYNC_CURSOR = 11,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_OFFLINE_OPERATION = 12,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_PRESENCE = 13,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_MIGRATION = 14,
    UMI_WORKBENCH_LAYOUT_DATA_RECORD_METADATA = 15
} UmiWorkbenchLayoutDataRecordKind;

/**
 * List the named workbench layout data change kind values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataChangeKind {
    UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED = 1,
    UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED = 2,
    UMI_WORKBENCH_LAYOUT_DATA_CHANGE_DELETED = 3,
    UMI_WORKBENCH_LAYOUT_DATA_CHANGE_SHARED = 4,
    UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UNSHARED = 5,
    UMI_WORKBENCH_LAYOUT_DATA_CHANGE_LOCKED = 6,
    UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UNLOCKED = 7,
    UMI_WORKBENCH_LAYOUT_DATA_CHANGE_SESSION_CHECKPOINTED = 8,
    UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CONFLICT_RECORDED = 9,
    UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CONFLICT_RESOLVED = 10
} UmiWorkbenchLayoutDataChangeKind;

/**
 * List the named workbench layout data permission values accepted by this public contract.
 */
typedef enum UmiWorkbenchLayoutDataPermission {
    UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW = 1,
    UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_EDIT = 2,
    UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_SHARE = 3,
    UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_DELETE = 4,
    UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_LOCK = 5,
    UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_ADMINISTER = 6
} UmiWorkbenchLayoutDataPermission;

/**
 * List the named workbench layout data lease state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataLeaseState {
    UMI_WORKBENCH_LAYOUT_DATA_LEASE_AVAILABLE = 1,
    UMI_WORKBENCH_LAYOUT_DATA_LEASE_HELD = 2,
    UMI_WORKBENCH_LAYOUT_DATA_LEASE_EXPIRED = 3,
    UMI_WORKBENCH_LAYOUT_DATA_LEASE_RELEASED = 4,
    UMI_WORKBENCH_LAYOUT_DATA_LEASE_REVOKED = 5
} UmiWorkbenchLayoutDataLeaseState;

/**
 * List the named workbench layout data outbox state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataOutboxState {
    UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_PENDING = 1,
    UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_LEASED = 2,
    UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DELIVERED = 3,
    UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_RETRY_WAIT = 4,
    UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DEAD_LETTER = 5
} UmiWorkbenchLayoutDataOutboxState;

/**
 * List the named workbench layout data conflict kind values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataConflictKind {
    UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_CONCURRENT_UPDATE = 1,
    UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_DELETE_VERSUS_UPDATE = 2,
    UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_PERMISSION = 3,
    UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_SCHEMA = 4,
    UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_INTEGRITY = 5,
    UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_MISSING_BASE = 6
} UmiWorkbenchLayoutDataConflictKind;

/**
 * List the named workbench layout data conflict state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataConflictState {
    UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_OPEN = 1,
    UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_LOCAL = 2,
    UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_REMOTE = 3,
    UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_MERGED = 4,
    UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_DISMISSED = 5
} UmiWorkbenchLayoutDataConflictState;

/**
 * List the named workbench layout data sync direction values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataSyncDirection {
    UMI_WORKBENCH_LAYOUT_DATA_SYNC_PULL = 1,
    UMI_WORKBENCH_LAYOUT_DATA_SYNC_PUSH = 2,
    UMI_WORKBENCH_LAYOUT_DATA_SYNC_BIDIRECTIONAL = 3
} UmiWorkbenchLayoutDataSyncDirection;

/**
 * List the named workbench layout data sync action values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataSyncAction {
    UMI_WORKBENCH_LAYOUT_DATA_SYNC_NO_ACTION = 1,
    UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_LOCAL = 2,
    UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_LOCAL = 3,
    UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_LOCAL = 4,
    UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_REMOTE = 5,
    UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_REMOTE = 6,
    UMI_WORKBENCH_LAYOUT_DATA_SYNC_DELETE_REMOTE = 7,
    UMI_WORKBENCH_LAYOUT_DATA_SYNC_RECORD_CONFLICT = 8
} UmiWorkbenchLayoutDataSyncAction;

/**
 * List the named workbench layout data operation state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataOperationState {
    UMI_WORKBENCH_LAYOUT_DATA_OPERATION_PENDING = 1,
    UMI_WORKBENCH_LAYOUT_DATA_OPERATION_CLAIMED = 2,
    UMI_WORKBENCH_LAYOUT_DATA_OPERATION_COMPLETED = 3,
    UMI_WORKBENCH_LAYOUT_DATA_OPERATION_RETRY_WAIT = 4,
    UMI_WORKBENCH_LAYOUT_DATA_OPERATION_FAILED = 5,
    UMI_WORKBENCH_LAYOUT_DATA_OPERATION_CANCELLED = 6
} UmiWorkbenchLayoutDataOperationState;

/**
 * List the named workbench layout data migration state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataMigrationState {
    UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_PENDING = 1,
    UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_RUNNING = 2,
    UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_APPLIED = 3,
    UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_FAILED = 4,
    UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_ROLLED_BACK = 5
} UmiWorkbenchLayoutDataMigrationState;

/**
 * List the named workbench layout data health state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataHealthState {
    UMI_WORKBENCH_LAYOUT_DATA_HEALTH_HEALTHY = 1,
    UMI_WORKBENCH_LAYOUT_DATA_HEALTH_DEGRADED = 2,
    UMI_WORKBENCH_LAYOUT_DATA_HEALTH_UNHEALTHY = 3,
    UMI_WORKBENCH_LAYOUT_DATA_HEALTH_UNAVAILABLE = 4
} UmiWorkbenchLayoutDataHealthState;

/**
 * List the named workbench layout data controller state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutDataControllerState {
    UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_CREATED = 1,
    UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_INITIALISED = 2,
    UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_RUNNING = 3,
    UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_QUIESCED = 4,
    UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_STOPPED = 5,
    UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_FAILED = 6
} UmiWorkbenchLayoutDataControllerState;

/**
 * Represent the workbench layout data record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutDataRecord {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordKind kind;
    char record_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char aggregate_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char owner_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char workspace_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t sequence;
    uint64_t created_at_ms;
    uint64_t modified_at_ms;
    uint64_t content_hash;
    uint32_t flags;
} UmiWorkbenchLayoutDataRecord;

/**
 * Represent the workbench layout data chunk manifest data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutDataChunkManifest {
    uint32_t structure_size;
    char aggregate_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    UmiWorkbenchLayoutDataRecordKind manifest_kind;
    UmiWorkbenchLayoutDataRecordKind chunk_kind;
    size_t chunk_count;
    size_t byte_count;
    uint64_t content_hash;
    uint64_t revision;
    uint64_t modified_at_ms;
} UmiWorkbenchLayoutDataChunkManifest;

/**
 * Represent the workbench layout revision record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutRevisionRecord {
    uint32_t structure_size;
    char revision_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char actor_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char correlation_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t base_revision;
    uint64_t content_hash;
    uint64_t created_at_ms;
    bool deletion;
    char summary[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutRevisionRecord;

/**
 * Represent the workbench layout grant data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchLayoutGrant {
    uint32_t structure_size;
    char grant_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char subject_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char granted_by[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint64_t granted_at_ms;
    uint64_t expires_at_ms;
    uint32_t permissions;
    bool inherited;
    bool revoked;
} UmiWorkbenchLayoutGrant;

/**
 * Represent the workbench layout lease data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchLayoutLease {
    uint32_t structure_size;
    char lease_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char holder_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char client_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    UmiWorkbenchLayoutDataLeaseState state;
    uint64_t fencing_token;
    uint64_t acquired_at_ms;
    uint64_t renewed_at_ms;
    uint64_t expires_at_ms;
    uint64_t revision;
} UmiWorkbenchLayoutLease;

/**
 * Represent the workbench layout change data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchLayoutChange {
    uint32_t structure_size;
    char change_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char actor_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char correlation_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    UmiWorkbenchLayoutDataChangeKind kind;
    uint64_t sequence;
    uint64_t revision;
    uint64_t base_revision;
    uint64_t content_hash;
    uint64_t occurred_at_ms;
    char summary[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutChange;

/**
 * Represent the workbench layout outbox entry data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutOutboxEntry {
    uint32_t structure_size;
    char entry_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char topic[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char aggregate_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char correlation_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    UmiWorkbenchLayoutDataOutboxState state;
    uint64_t sequence;
    uint64_t created_at_ms;
    uint64_t available_at_ms;
    uint64_t leased_until_ms;
    uint32_t attempt_count;
    uint32_t maximum_attempts;
    char payload_reference[UMI_WORKBENCH_LAYOUT_DATA_PATH_CAPACITY];
    char last_error[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutOutboxEntry;

/**
 * Represent the workbench layout conflict data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutConflict {
    uint32_t structure_size;
    char conflict_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char local_replica_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char remote_replica_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    UmiWorkbenchLayoutDataConflictKind kind;
    UmiWorkbenchLayoutDataConflictState state;
    uint64_t base_revision;
    uint64_t local_revision;
    uint64_t remote_revision;
    uint64_t created_at_ms;
    uint64_t resolved_at_ms;
    char resolved_by[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char message[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutConflict;

/**
 * Represent the workbench layout sync cursor data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutSyncCursor {
    uint32_t structure_size;
    char cursor_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char replica_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char peer_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint64_t last_pulled_sequence;
    uint64_t last_pushed_sequence;
    uint64_t last_success_at_ms;
    uint64_t last_attempt_at_ms;
    uint64_t revision;
    char last_error[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutSyncCursor;

/**
 * Represent the workbench layout sync plan item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutSyncPlanItem {
    uint32_t structure_size;
    char layout_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char change_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    UmiWorkbenchLayoutDataSyncAction action;
    uint64_t local_revision;
    uint64_t remote_revision;
    uint64_t sequence;
    uint64_t content_hash;
    bool requires_lease;
    bool requires_permission_check;
    char reason[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutSyncPlanItem;

/**
 * Represent the workbench layout sync plan data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutSyncPlan {
    uint32_t structure_size;
    char plan_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char local_replica_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char remote_replica_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    UmiWorkbenchLayoutDataSyncDirection direction;
    UmiWorkbenchLayoutSyncPlanItem items[UMI_WORKBENCH_LAYOUT_DATA_MAX_SYNC_ITEMS];
    size_t item_count;
    size_t conflict_count;
    size_t push_count;
    size_t pull_count;
    uint64_t from_sequence;
    uint64_t to_sequence;
    uint64_t created_at_ms;
} UmiWorkbenchLayoutSyncPlan;

/**
 * Represent the workbench layout offline operation data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutOfflineOperation {
    uint32_t structure_size;
    char operation_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char actor_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char correlation_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    UmiWorkbenchLayoutDataChangeKind change_kind;
    UmiWorkbenchLayoutDataOperationState state;
    uint64_t expected_revision;
    uint64_t created_at_ms;
    uint64_t available_at_ms;
    uint64_t claimed_until_ms;
    uint32_t attempt_count;
    uint32_t maximum_attempts;
    char payload_reference[UMI_WORKBENCH_LAYOUT_DATA_PATH_CAPACITY];
    char last_error[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutOfflineOperation;

/**
 * Represent the workbench layout presence data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutPresence {
    uint32_t structure_size;
    char presence_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char user_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char client_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char active_node_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint64_t connected_at_ms;
    uint64_t last_seen_at_ms;
    uint64_t expires_at_ms;
    bool editing;
    bool synchronising;
} UmiWorkbenchLayoutPresence;

/**
 * Represent the workbench layout data metrics data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutDataMetrics {
    uint32_t structure_size;
    uint64_t save_count;
    uint64_t load_count;
    uint64_t delete_count;
    uint64_t conflict_count;
    uint64_t sync_plan_count;
    uint64_t sync_success_count;
    uint64_t sync_failure_count;
    uint64_t outbox_delivery_count;
    uint64_t outbox_retry_count;
    uint64_t offline_replay_count;
    uint64_t bytes_written;
    uint64_t bytes_read;
    uint64_t last_operation_duration_us;
    uint64_t maximum_operation_duration_us;
    uint64_t revision;
} UmiWorkbenchLayoutDataMetrics;

/**
 * Represent the workbench layout data health snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutDataHealthSnapshot {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataHealthState state;
    char backend_name[64];
    char backend_path[UMI_WORKBENCH_LAYOUT_DATA_PATH_CAPACITY];
    size_t record_count;
    size_t pending_outbox_count;
    size_t open_conflict_count;
    size_t pending_offline_operation_count;
    size_t active_presence_count;
    uint64_t last_success_at_ms;
    uint64_t last_failure_at_ms;
    uint64_t revision;
    char message[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutDataHealthSnapshot;

/**
 * Represent the workbench layout migration descriptor data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutMigrationDescriptor {
    uint32_t structure_size;
    char migration_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char name[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint32_t from_version;
    uint32_t to_version;
    uint64_t checksum;
    bool reversible;
    bool requires_exclusive_access;
} UmiWorkbenchLayoutMigrationDescriptor;

/**
 * Represent the workbench layout migration record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutMigrationRecord {
    uint32_t structure_size;
    UmiWorkbenchLayoutMigrationDescriptor descriptor;
    UmiWorkbenchLayoutDataMigrationState state;
    char applied_by[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint64_t started_at_ms;
    uint64_t completed_at_ms;
    char error[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutMigrationRecord;

/**
 * Represent the workbench layout backup manifest data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutBackupManifest {
    uint32_t structure_size;
    char backup_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char source_replica_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint32_t schema_version;
    size_t record_count;
    size_t byte_count;
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t content_hash;
    uint64_t created_at_ms;
    bool includes_sessions;
    bool includes_audit;
    bool includes_outbox;
} UmiWorkbenchLayoutBackupManifest;

/**
 * Represent the workbench layout reconciliation issue data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutReconciliationIssue {
    uint32_t structure_size;
    char issue_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    UmiStatus status;
    uint64_t detected_at_ms;
    bool repairable;
    char message[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutReconciliationIssue;

/**
 * Represent the workbench layout reconciliation report data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutReconciliationReport {
    uint32_t structure_size;
    UmiWorkbenchLayoutReconciliationIssue
        issues[UMI_WORKBENCH_LAYOUT_DATA_MAX_RECONCILIATION_ISSUES];
    size_t issue_count;
    size_t record_count;
    size_t manifest_count;
    size_t chunk_count;
    size_t orphan_count;
    size_t repaired_count;
    uint64_t started_at_ms;
    uint64_t completed_at_ms;
    bool consistent;
} UmiWorkbenchLayoutReconciliationReport;

/**
 * Provide the workbench layout data record kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_record_kind_text(
    UmiWorkbenchLayoutDataRecordKind kind);
/**
 * Provide the workbench layout data change kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_change_kind_text(
    UmiWorkbenchLayoutDataChangeKind kind);
/**
 * Provide the workbench layout data permission text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_permission_text(
    UmiWorkbenchLayoutDataPermission permission);
/**
 * Provide the workbench layout data lease state text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_lease_state_text(
    UmiWorkbenchLayoutDataLeaseState state);
/**
 * Provide the workbench layout data outbox state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_outbox_state_text(
    UmiWorkbenchLayoutDataOutboxState state);
/**
 * Provide the workbench layout data conflict kind text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_conflict_kind_text(
    UmiWorkbenchLayoutDataConflictKind kind);
/**
 * Provide the workbench layout data conflict state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_conflict_state_text(
    UmiWorkbenchLayoutDataConflictState state);
/**
 * Provide the workbench layout data sync action text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_sync_action_text(
    UmiWorkbenchLayoutDataSyncAction action);
/**
 * Provide the workbench layout data operation state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_operation_state_text(
    UmiWorkbenchLayoutDataOperationState state);
/**
 * Provide the workbench layout data health state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_health_state_text(
    UmiWorkbenchLayoutDataHealthState state);
/**
 * Provide the workbench layout data controller state text operation used by this module
 * and its client applications.
 */
const char *umi_workbench_layout_data_controller_state_text(
    UmiWorkbenchLayoutDataControllerState state);

/**
 * Provide the workbench layout data hash bytes operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_layout_data_hash_bytes(
    const void *data,
    size_t size);
/**
 * Provide the workbench layout data hash text operation used by this module and its client
 * applications.
 */
uint64_t umi_workbench_layout_data_hash_text(
    const char *text);
/**
 * Provide the workbench layout data copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_data_copy_text(
    char *destination,
    size_t capacity,
    const char *source,
    bool allow_empty);
/**
 * Provide the workbench layout data text equal operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_data_text_equal(
    const char *left,
    const char *right);
/**
 * Provide the workbench layout data text has prefix operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_data_text_has_prefix(
    const char *text,
    const char *prefix);
/**
 * Provide the workbench layout data time expired operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_data_time_expired(
    uint64_t expires_at_ms,
    uint64_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
