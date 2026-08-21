/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/retention.h
 *
 * PURPOSE:
 *   Prune completed transient layout records under explicit retention policy while preserving authoritative layouts and audit evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_RETENTION_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_RETENTION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutRetentionPolicy {
    uint32_t structure_size;
    size_t revisions_per_layout;
    uint64_t delivered_outbox_retention_ms;
    uint64_t resolved_conflict_retention_ms;
    uint64_t completed_operation_retention_ms;
    uint64_t expired_presence_retention_ms;
    bool preserve_deletion_revisions;
    bool dry_run;
} UmiWorkbenchLayoutRetentionPolicy;

typedef struct UmiWorkbenchLayoutRetentionResult {
    uint32_t structure_size;
    UmiStatus status;
    size_t examined_count;
    size_t eligible_count;
    size_t deleted_count;
    size_t preserved_count;
    char failed_key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char message[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutRetentionResult;

UmiWorkbenchLayoutRetentionPolicy
umi_workbench_layout_retention_policy_default(void);

UmiStatus umi_workbench_layout_retention_apply(
    UmiDataServer *server,
    const UmiWorkbenchLayoutRetentionPolicy *policy,
    uint64_t now_ms,
    UmiWorkbenchLayoutRetentionResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
