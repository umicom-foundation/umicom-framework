/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/reconciliation.c
 *
 * PURPOSE:
 *   Implement Data Server reconciliation, payload integrity checks, orphan detection and safe repair evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/reconciliation.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include <stdio.h>
#include <string.h>
#include "internal.h"




typedef struct ReconcileScan {
    UmiDataServer *server;
    UmiWorkbenchLayoutReconciliationPolicy policy;
    UmiWorkbenchLayoutReconciliationReport *report;
    uint64_t now_ms;
    UmiWorkbenchLayoutDataChunkManifest
        layout_manifests[UMI_WORKBENCH_LAYOUT_DATA_MAX_RECONCILIATION_ISSUES];
    size_t layout_manifest_count;
    UmiWorkbenchLayoutDataChunkManifest
        session_manifests[UMI_WORKBENCH_LAYOUT_DATA_MAX_RECONCILIATION_ISSUES];
    size_t session_manifest_count;
    char chunk_keys[UMI_WORKBENCH_LAYOUT_DATA_MAX_RECONCILIATION_ISSUES]
                   [UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    size_t chunk_key_count;
} ReconcileScan;

/*
 * Provide the workbench layout reconciliation policy default operation used by this module
 * and its client applications.
 */
UmiWorkbenchLayoutReconciliationPolicy
umi_workbench_layout_reconciliation_policy_default(void)
{
    UmiWorkbenchLayoutReconciliationPolicy policy;
    (void)memset(&policy, 0, sizeof(policy));
    policy.structure_size = sizeof(policy);
    policy.verify_layout_payloads = true;
    policy.verify_session_payloads = true;
    policy.detect_orphan_chunks = true;
    policy.detect_unknown_keys = true;
    policy.repair_orphan_chunks = false;
    policy.stop_after_capacity = true;
    return policy;
}

/*
 * Provide the workbench layout reconciliation add issue operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_reconciliation_add_issue(
    UmiWorkbenchLayoutReconciliationReport *report,
    const char *key,
    UmiStatus status,
    bool repairable,
    const char *message,
    uint64_t detected_at_ms)
{
    UmiWorkbenchLayoutReconciliationIssue *issue;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL || key == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (report->issue_count >=
        UMI_WORKBENCH_LAYOUT_DATA_MAX_RECONCILIATION_ISSUES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    issue = &report->issues[report->issue_count++];
    (void)memset(issue, 0, sizeof(*issue));
    issue->structure_size = sizeof(*issue);
    issue->status = status;
    issue->detected_at_ms = detected_at_ms;
    issue->repairable = repairable;
    (void)umi_workbench_layout_data_copy_text(
        issue->key, sizeof(issue->key), key, true);
    (void)umi_workbench_layout_data_copy_text(
        issue->message, sizeof(issue->message), message, true);
    report->consistent = false;
    return UMI_STATUS_OK;
}

/* Provide the remember manifest operation used by this module and its client applications. */
static UmiStatus remember_manifest(
    UmiWorkbenchLayoutDataChunkManifest *manifests,
    size_t *count,
    const char *value)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*count >=
        UMI_WORKBENCH_LAYOUT_DATA_MAX_RECONCILIATION_ISSUES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_chunk_manifest_decode(
            value, &manifests[*count]) != UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *count += 1U;
    return UMI_STATUS_OK;
}

/* Provide the scan accept operation used by this module and its client applications. */
static UmiStatus scan_accept(
    const char *key,
    const char *value,
    void *context)
{
    ReconcileScan *scan = (ReconcileScan *)context;
    UmiWorkbenchLayoutDataKeyParts parts;
    UmiStatus status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_workbench_layout_data_key_is_owned(key)) {
        return UMI_STATUS_OK;
    }
    scan->report->record_count += 1U;
    status = umi_workbench_layout_data_key_parse(key, &parts);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)umi_workbench_layout_reconciliation_add_issue(
            scan->report, key, status, false,
            "The Data Server key is not a recognised layout record.",
            scan->now_ms);
        return scan->policy.stop_after_capacity &&
               scan->report->issue_count >=
                   UMI_WORKBENCH_LAYOUT_DATA_MAX_RECONCILIATION_ISSUES
            ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    }
    /* Select the behaviour associated with the requested command or state value. */
    switch (parts.kind) {
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST:
        scan->report->manifest_count += 1U;
        status = remember_manifest(
            scan->layout_manifests,
            &scan->layout_manifest_count, value);
        break;
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST:
        scan->report->manifest_count += 1U;
        status = remember_manifest(
            scan->session_manifests,
            &scan->session_manifest_count, value);
        break;
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK:
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK:
        scan->report->chunk_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (scan->chunk_key_count <
            UMI_WORKBENCH_LAYOUT_DATA_MAX_RECONCILIATION_ISSUES) {
            (void)umi_workbench_layout_data_copy_text(
                scan->chunk_keys[scan->chunk_key_count],
                sizeof(scan->chunk_keys[scan->chunk_key_count]),
                key, false);
            scan->chunk_key_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        }
        break;
    default:
        status = UMI_STATUS_OK;
        break;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)umi_workbench_layout_reconciliation_add_issue(
            scan->report, key, status, false,
            "The layout record could not be decoded during reconciliation.",
            scan->now_ms);
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_OK;
}

/* Provide the find manifest operation used by this module and its client applications. */
static const UmiWorkbenchLayoutDataChunkManifest *find_manifest(
    const ReconcileScan *scan,
    UmiWorkbenchLayoutDataRecordKind chunk_kind,
    const char *aggregate_id)
{
    const UmiWorkbenchLayoutDataChunkManifest *manifests;
    size_t count;
    size_t index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (chunk_kind ==
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK) {
        manifests = scan->layout_manifests;
        count = scan->layout_manifest_count;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        manifests = scan->session_manifests;
        count = scan->session_manifest_count;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(manifests[index].aggregate_id,
                   aggregate_id) == 0) {
            return &manifests[index];
        }
    }
    return NULL;
}

/* Provide the verify payloads operation used by this module and its client applications. */
static UmiStatus verify_payloads(ReconcileScan *scan)
{
    UmiWorkbenchLayoutChunkStore store;
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (scan->policy.verify_layout_payloads) {
        (void)umi_workbench_layout_chunk_store_init(
            &store, scan->server,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK);
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U;
             index < scan->layout_manifest_count;
             ++index) {
            const char *id =
                scan->layout_manifests[index].aggregate_id;
            status = umi_workbench_layout_chunk_store_verify(
                &store, id, NULL);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                (void)umi_workbench_layout_reconciliation_add_issue(
                    scan->report, id, status, false,
                    "The persisted layout payload failed integrity verification.",
                    scan->now_ms);
                status = UMI_STATUS_OK;
            }
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (scan->policy.verify_session_payloads) {
        (void)umi_workbench_layout_chunk_store_init(
            &store, scan->server,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK);
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U;
             index < scan->session_manifest_count;
             ++index) {
            const char *id =
                scan->session_manifests[index].aggregate_id;
            status = umi_workbench_layout_chunk_store_verify(
                &store, id, NULL);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                (void)umi_workbench_layout_reconciliation_add_issue(
                    scan->report, id, status, false,
                    "The persisted session payload failed integrity verification.",
                    scan->now_ms);
                status = UMI_STATUS_OK;
            }
        }
    }
    return status;
}

/* Provide the detect orphans operation used by this module and its client applications. */
static UmiStatus detect_orphans(ReconcileScan *scan)
{
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!scan->policy.detect_orphan_chunks) return UMI_STATUS_OK;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < scan->chunk_key_count; ++index) {
        UmiWorkbenchLayoutDataKeyParts parts;
        const UmiWorkbenchLayoutDataChunkManifest *manifest;
        status = umi_workbench_layout_data_key_parse(
            scan->chunk_keys[index], &parts);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) continue;
        manifest = find_manifest(
            scan, parts.kind, parts.aggregate_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (manifest == NULL ||
            parts.chunk_index >= manifest->chunk_count) {
            scan->report->orphan_count += 1U;
            (void)umi_workbench_layout_reconciliation_add_issue(
                scan->report, scan->chunk_keys[index],
                UMI_STATUS_NOT_FOUND, true,
                "The payload chunk has no matching manifest.",
                scan->now_ms);
            /* Apply this branch only when its contract condition is satisfied. */
            if (scan->policy.repair_orphan_chunks) {
                status = umi_data_server_delete(
                    scan->server, scan->chunk_keys[index]);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    scan->report->repaired_count += 1U;
                } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_NOT_FOUND) {
                    return status;
                }
            }
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout reconcile operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_reconcile(
    UmiDataServer *server,
    const UmiWorkbenchLayoutReconciliationPolicy *policy,
    uint64_t now_ms,
    UmiWorkbenchLayoutReconciliationReport *out_report)
{
    ReconcileScan scan;
    UmiWorkbenchLayoutReconciliationPolicy effective;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = policy != NULL
        ? *policy : umi_workbench_layout_reconciliation_policy_default();
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.structure_size < sizeof(effective)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->structure_size = sizeof(*out_report);
    out_report->consistent = true;
    out_report->started_at_ms = now_ms;
    (void)memset(&scan, 0, sizeof(scan));
    scan.server = server;
    scan.policy = effective;
    scan.report = out_report;
    scan.now_ms = now_ms;
    status = umi_data_server_visit(server, scan_accept, &scan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = verify_payloads(&scan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = detect_orphans(&scan);
    out_report->completed_at_ms = now_ms;
    return status;
}
