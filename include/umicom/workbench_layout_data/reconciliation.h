/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/reconciliation.h
 *
 * PURPOSE:
 *   Reconcile manifests, chunks and record keys, report integrity defects and optionally repair safe orphan records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_RECONCILIATION_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_RECONCILIATION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/chunk_store.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutReconciliationPolicy {
    uint32_t structure_size;
    bool verify_layout_payloads;
    bool verify_session_payloads;
    bool detect_orphan_chunks;
    bool detect_unknown_keys;
    bool repair_orphan_chunks;
    bool stop_after_capacity;
} UmiWorkbenchLayoutReconciliationPolicy;

UmiWorkbenchLayoutReconciliationPolicy
umi_workbench_layout_reconciliation_policy_default(void);

UmiStatus umi_workbench_layout_reconcile(
    UmiDataServer *server,
    const UmiWorkbenchLayoutReconciliationPolicy *policy,
    uint64_t now_ms,
    UmiWorkbenchLayoutReconciliationReport *out_report);

UmiStatus umi_workbench_layout_reconciliation_add_issue(
    UmiWorkbenchLayoutReconciliationReport *report,
    const char *key,
    UmiStatus status,
    bool repairable,
    const char *message,
    uint64_t detected_at_ms);

#ifdef __cplusplus
}
#endif

#endif
