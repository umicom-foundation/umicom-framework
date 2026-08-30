/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/health.c
 *
 * PURPOSE:
 *   Implement readiness and liveness policy for authoritative layout persistence and replication services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/health.h"
#include <string.h>



UmiWorkbenchLayoutHealthPolicy
umi_workbench_layout_health_policy_default(void)
{
    UmiWorkbenchLayoutHealthPolicy policy;
    (void)memset(&policy, 0, sizeof(policy));
    policy.structure_size = sizeof(policy);
    policy.maximum_pending_outbox = 100U;
    policy.maximum_open_conflicts = 50U;
    policy.maximum_pending_offline_operations = 100U;
    policy.maximum_failure_age_ms = UINT64_C(300000);
    policy.require_sqlite_for_production = false;
    return policy;
}

UmiStatus umi_workbench_layout_health_evaluate(
    const UmiDataServer *server,
    const UmiWorkbenchLayoutDataMetrics *metrics,
    size_t pending_outbox,
    size_t open_conflicts,
    size_t pending_offline_operations,
    size_t active_presence,
    uint64_t now_ms,
    uint64_t last_success_at_ms,
    uint64_t last_failure_at_ms,
    const UmiWorkbenchLayoutHealthPolicy *policy,
    UmiWorkbenchLayoutDataHealthSnapshot *out_snapshot)
{
    UmiWorkbenchLayoutHealthPolicy effective;
    UmiDataServerSnapshot data;
    UmiStatus status;
    if (server == NULL || metrics == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = policy != NULL
        ? *policy : umi_workbench_layout_health_policy_default();
    if (effective.structure_size < sizeof(effective)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = sizeof(*out_snapshot);
    status = umi_data_server_snapshot(server, &data);
    if (status != UMI_STATUS_OK) {
        out_snapshot->state =
            UMI_WORKBENCH_LAYOUT_DATA_HEALTH_UNAVAILABLE;
        (void)umi_workbench_layout_data_copy_text(
            out_snapshot->message, sizeof(out_snapshot->message),
            "The authoritative Data Server snapshot is unavailable.", true);
        return status;
    }
    (void)umi_workbench_layout_data_copy_text(
        out_snapshot->backend_name,
        sizeof(out_snapshot->backend_name),
        data.backend_name, true);
    (void)umi_workbench_layout_data_copy_text(
        out_snapshot->backend_path,
        sizeof(out_snapshot->backend_path),
        data.path, true);
    out_snapshot->record_count = data.record_count;
    out_snapshot->pending_outbox_count = pending_outbox;
    out_snapshot->open_conflict_count = open_conflicts;
    out_snapshot->pending_offline_operation_count =
        pending_offline_operations;
    out_snapshot->active_presence_count = active_presence;
    out_snapshot->last_success_at_ms = last_success_at_ms;
    out_snapshot->last_failure_at_ms = last_failure_at_ms;
    out_snapshot->revision = metrics->revision;

    if (effective.require_sqlite_for_production &&
        data.backend != UMI_DATA_BACKEND_SQLITE) {
        out_snapshot->state =
            UMI_WORKBENCH_LAYOUT_DATA_HEALTH_UNHEALTHY;
        (void)umi_workbench_layout_data_copy_text(
            out_snapshot->message, sizeof(out_snapshot->message),
            "Production policy requires the SQLite Data Server backend.",
            true);
    } else if (open_conflicts >
                   effective.maximum_open_conflicts) {
        out_snapshot->state =
            UMI_WORKBENCH_LAYOUT_DATA_HEALTH_UNHEALTHY;
        (void)umi_workbench_layout_data_copy_text(
            out_snapshot->message, sizeof(out_snapshot->message),
            "Open layout conflicts exceed the safe synchronisation threshold.",
            true);
    } else if (pending_outbox >
                   effective.maximum_pending_outbox ||
               pending_offline_operations >
                   effective.maximum_pending_offline_operations) {
        out_snapshot->state =
            UMI_WORKBENCH_LAYOUT_DATA_HEALTH_DEGRADED;
        (void)umi_workbench_layout_data_copy_text(
            out_snapshot->message, sizeof(out_snapshot->message),
            "Layout persistence is available but operational queues exceed policy.",
            true);
    } else if (last_failure_at_ms > last_success_at_ms &&
               now_ms >= last_failure_at_ms &&
               now_ms - last_failure_at_ms <
                   effective.maximum_failure_age_ms) {
        out_snapshot->state =
            UMI_WORKBENCH_LAYOUT_DATA_HEALTH_DEGRADED;
        (void)umi_workbench_layout_data_copy_text(
            out_snapshot->message, sizeof(out_snapshot->message),
            "A recent persistence or synchronisation operation failed.",
            true);
    } else {
        out_snapshot->state =
            UMI_WORKBENCH_LAYOUT_DATA_HEALTH_HEALTHY;
        (void)umi_workbench_layout_data_copy_text(
            out_snapshot->message, sizeof(out_snapshot->message),
            "Layout persistence and synchronisation are healthy.", true);
    }
    return UMI_STATUS_OK;
}

bool umi_workbench_layout_health_ready(
    const UmiWorkbenchLayoutDataHealthSnapshot *snapshot)
{
    return snapshot != NULL &&
        (snapshot->state ==
             UMI_WORKBENCH_LAYOUT_DATA_HEALTH_HEALTHY ||
         snapshot->state ==
             UMI_WORKBENCH_LAYOUT_DATA_HEALTH_DEGRADED);
}

bool umi_workbench_layout_health_live(
    const UmiWorkbenchLayoutDataHealthSnapshot *snapshot)
{
    return snapshot != NULL &&
        snapshot->state !=
            UMI_WORKBENCH_LAYOUT_DATA_HEALTH_UNAVAILABLE;
}
