/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/workspace_checkpoint.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_WORKSPACE_CHECKPOINT_H
#define UMICOM_APPLICATION_PRODUCTION_WORKSPACE_CHECKPOINT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/runtime/session_snapshot.h"
#include "umicom/application/runtime/workspace_runtime.h"

typedef struct UmiApplicationProductionWorkspaceCheckpoint {
    uint64_t sequence;
    UmiApplicationSessionSnapshot session;
    char reason[UMI_APPLICATION_PRODUCTION_TEXT_CAPACITY];
    int clean_shutdown;
} UmiApplicationProductionWorkspaceCheckpoint;

UmiStatus umi_application_production_workspace_checkpoint_capture(
    const UmiApplicationWorkspaceRuntime *runtime, uint64_t sequence,
    const char *reason, int clean_shutdown,
    UmiApplicationProductionWorkspaceCheckpoint *out_checkpoint);
UmiStatus umi_application_production_workspace_checkpoint_validate(
    const UmiApplicationProductionWorkspaceCheckpoint *checkpoint);

#ifdef __cplusplus
}
#endif
#endif
