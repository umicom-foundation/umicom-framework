/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/diagnostics.h
 *
 * PURPOSE:
 *   Summarise layout health, complexity, ownership, geometry and recovery readiness for diagnostics panes and automated quality gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DIAGNOSTICS_H
#define UMICOM_WORKBENCH_LAYOUT_DIAGNOSTICS_H

#include "umicom/workbench_layout/monitor_topology.h"
#include "umicom/workbench_layout/recovery.h"
#include "umicom/workbench_layout/validation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchLayoutHealth {
    uint32_t structure_size;
    bool valid;
    bool connected;
    bool geometry_safe;
    bool hash_valid;
    bool recovery_ready;
    size_t node_count;
    size_t container_count;
    size_t panel_count;
    size_t floating_window_count;
    size_t application_owner_count;
    size_t context_group_count;
    size_t offscreen_window_count;
    size_t warning_count;
    size_t error_count;
    size_t maximum_depth;
    uint64_t content_hash;
    char summary[UMI_WORKBENCH_LAYOUT_DESCRIPTION_CAPACITY];
} UmiWorkbenchLayoutHealth;

UmiStatus umi_workbench_layout_diagnose(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchRecoveryPlan *recovery_plan,
    UmiWorkbenchLayoutHealth *out_health,
    UmiWorkbenchLayoutValidationReport *out_validation);

UmiStatus umi_workbench_layout_health_format(
    const UmiWorkbenchLayoutHealth *health,
    char *buffer,
    size_t capacity,
    size_t *out_required);

#ifdef __cplusplus
}
#endif

#endif
