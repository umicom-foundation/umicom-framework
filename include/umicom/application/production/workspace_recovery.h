/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/workspace_recovery.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_WORKSPACE_RECOVERY_H
#define UMICOM_APPLICATION_PRODUCTION_WORKSPACE_RECOVERY_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/checkpoint_store.h"

typedef struct UmiApplicationProductionRecoveryReport {
    int checkpoint_found;
    int restored;
    int clean_shutdown;
    uint64_t checkpoint_sequence;
    size_t restored_panel_count;
    UmiStatus status;
} UmiApplicationProductionRecoveryReport;

UmiStatus umi_application_production_workspace_recover(
    const UmiApplicationProductionCheckpointStore *store,
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationWorkspaceRuntime *out_runtime,
    UmiApplicationProductionRecoveryReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
