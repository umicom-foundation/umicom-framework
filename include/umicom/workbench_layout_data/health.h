/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/health.h
 *
 * PURPOSE:
 *   Derive readiness and liveness from Data Server state, queue pressure, conflicts and operation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_HEALTH_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_HEALTH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutHealthPolicy {
    uint32_t structure_size;
    size_t maximum_pending_outbox;
    size_t maximum_open_conflicts;
    size_t maximum_pending_offline_operations;
    uint64_t maximum_failure_age_ms;
    bool require_sqlite_for_production;
} UmiWorkbenchLayoutHealthPolicy;

UmiWorkbenchLayoutHealthPolicy
umi_workbench_layout_health_policy_default(void);

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
    UmiWorkbenchLayoutDataHealthSnapshot *out_snapshot);

bool umi_workbench_layout_health_ready(
    const UmiWorkbenchLayoutDataHealthSnapshot *snapshot);

bool umi_workbench_layout_health_live(
    const UmiWorkbenchLayoutDataHealthSnapshot *snapshot);

#ifdef __cplusplus
}
#endif

#endif
