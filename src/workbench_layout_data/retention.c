/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/retention.c
 *
 * PURPOSE:
 *   Implement policy-driven retention for transient queues, presence and resolved conflicts without deleting authoritative layout state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/retention.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include "umicom/workbench_layout_data/value_codec.h"
#include <string.h>
#include "internal.h"




typedef struct RetentionCandidate {
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    UmiWorkbenchLayoutDataRecordKind kind;
    uint64_t completed_at_ms;
    bool eligible;
} RetentionCandidate;

typedef struct RetentionScan {
    UmiWorkbenchLayoutRetentionPolicy policy;
    uint64_t now_ms;
    RetentionCandidate candidates[UMI_WORKBENCH_LAYOUT_DATA_MAX_BACKUP_RECORDS];
    size_t count;
    UmiWorkbenchLayoutRetentionResult *result;
} RetentionScan;

/*
 * Provide the workbench layout retention policy default operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutRetentionPolicy
umi_workbench_layout_retention_policy_default(void)
{
    UmiWorkbenchLayoutRetentionPolicy policy;
    (void)memset(&policy, 0, sizeof(policy));
    policy.structure_size = sizeof(policy);
    policy.revisions_per_layout = 64U;
    policy.delivered_outbox_retention_ms = UINT64_C(604800000);
    policy.resolved_conflict_retention_ms = UINT64_C(2592000000);
    policy.completed_operation_retention_ms = UINT64_C(604800000);
    policy.expired_presence_retention_ms = UINT64_C(300000);
    policy.preserve_deletion_revisions = true;
    policy.dry_run = false;
    return policy;
}

/* Provide the field u64 operation used by this module and its client applications. */
static uint64_t field_u64(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name)
{
    uint64_t value = 0U;
    (void)umi_workbench_layout_data_field_set_get_u64(
        fields, name, &value);
    return value;
}

/* Provide the field bool operation used by this module and its client applications. */
static bool field_bool(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name)
{
    bool value = false;
    (void)umi_workbench_layout_data_field_set_get_bool(
        fields, name, &value);
    return value;
}

/* Provide the retention accept operation used by this module and its client applications. */
static UmiStatus retention_accept(
    const char *key,
    const char *value,
    void *context)
{
    RetentionScan *scan = (RetentionScan *)context;
    UmiWorkbenchLayoutDataKeyParts parts;
    UmiWorkbenchLayoutDataFieldSet fields;
    RetentionCandidate *candidate;
    uint32_t state = 0U;
    uint64_t timestamp = 0U;
    bool eligible = false;
    UmiStatus status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_workbench_layout_data_key_is_owned(key)) {
        return UMI_STATUS_OK;
    }
    scan->result->examined_count += 1U;
    status = umi_workbench_layout_data_key_parse(key, &parts);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (scan->count >= UMI_WORKBENCH_LAYOUT_DATA_MAX_BACKUP_RECORDS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (parts.kind != UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX &&
        parts.kind != UMI_WORKBENCH_LAYOUT_DATA_RECORD_CONFLICT &&
        parts.kind !=
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_OFFLINE_OPERATION &&
        parts.kind != UMI_WORKBENCH_LAYOUT_DATA_RECORD_PRESENCE &&
        parts.kind != UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION) {
        return UMI_STATUS_OK;
    }
    status = umi_workbench_layout_data_value_decode(value, &fields);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return UMI_STATUS_OK;
    /* Select the behaviour associated with the requested command or state value. */
    switch (parts.kind) {
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX:
        (void)umi_workbench_layout_data_field_set_get_u32(
            &fields, "state", &state);
        timestamp = field_u64(&fields, "leased_until_ms");
        eligible =
            state == UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DELIVERED &&
            scan->now_ms >= timestamp &&
            scan->now_ms - timestamp >=
                scan->policy.delivered_outbox_retention_ms;
        break;
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_CONFLICT:
        (void)umi_workbench_layout_data_field_set_get_u32(
            &fields, "state", &state);
        timestamp = field_u64(&fields, "resolved_at_ms");
        eligible =
            state != UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_OPEN &&
            timestamp != 0U &&
            scan->now_ms >= timestamp &&
            scan->now_ms - timestamp >=
                scan->policy.resolved_conflict_retention_ms;
        break;
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_OFFLINE_OPERATION:
        (void)umi_workbench_layout_data_field_set_get_u32(
            &fields, "state", &state);
        timestamp = field_u64(&fields, "available_at_ms");
        eligible =
            (state ==
                 UMI_WORKBENCH_LAYOUT_DATA_OPERATION_COMPLETED ||
             state ==
                 UMI_WORKBENCH_LAYOUT_DATA_OPERATION_CANCELLED) &&
            scan->now_ms >= timestamp &&
            scan->now_ms - timestamp >=
                scan->policy.completed_operation_retention_ms;
        break;
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_PRESENCE:
        timestamp = field_u64(&fields, "expires_at_ms");
        eligible =
            timestamp != 0U &&
            scan->now_ms >= timestamp &&
            scan->now_ms - timestamp >=
                scan->policy.expired_presence_retention_ms;
        break;
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION:
        eligible = !scan->policy.preserve_deletion_revisions ||
                   !field_bool(&fields, "deletion");
        break;
    default:
        break;
    }
    candidate = &scan->candidates[scan->count++];
    (void)memset(candidate, 0, sizeof(*candidate));
    candidate->kind = parts.kind;
    candidate->completed_at_ms = timestamp;
    candidate->eligible = eligible;
    (void)umi_workbench_layout_data_copy_text(
        candidate->key, sizeof(candidate->key), key, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (eligible) scan->result->eligible_count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Perform workbench layout retention through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_workbench_layout_retention_apply(
    UmiDataServer *server,
    const UmiWorkbenchLayoutRetentionPolicy *policy,
    uint64_t now_ms,
    UmiWorkbenchLayoutRetentionResult *out_result)
{
    UmiWorkbenchLayoutRetentionPolicy effective;
    UmiWorkbenchLayoutRetentionResult result;
    RetentionScan scan;
    size_t index;
    bool transaction_started = false;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = policy != NULL
        ? *policy : umi_workbench_layout_retention_policy_default();
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.structure_size < sizeof(effective)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&result, 0, sizeof(result));
    result.structure_size = sizeof(result);
    (void)memset(&scan, 0, sizeof(scan));
    scan.policy = effective;
    scan.now_ms = now_ms;
    scan.result = &result;
    status = umi_data_server_visit(server, retention_accept, &scan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && !effective.dry_run) {
        status = umi_workbench_layout_data_transaction_begin(
            server, &transaction_started);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < scan.count;
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!scan.candidates[index].eligible) {
            result.preserved_count += 1U;
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (effective.dry_run) continue;
        status = umi_data_server_delete(
            server, scan.candidates[index].key);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            result.deleted_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            (void)umi_workbench_layout_data_copy_text(
                result.failed_key, sizeof(result.failed_key),
                scan.candidates[index].key, true);
        }
    }
    status = umi_workbench_layout_data_transaction_finish(
        server, transaction_started, status);
    result.status = status;
    (void)umi_workbench_layout_data_copy_text(
        result.message, sizeof(result.message),
        status == UMI_STATUS_OK
            ? effective.dry_run
                ? "Layout retention dry run completed."
                : "Eligible transient layout records were pruned."
            : "Layout retention failed and was rolled back.",
        true);
    *out_result = result;
    return status;
}
