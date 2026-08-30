/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/runtime.h
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
#ifndef UMICOM_APPLICATION_PRODUCTION_RUNTIME_H
#define UMICOM_APPLICATION_PRODUCTION_RUNTIME_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/acceptance_report.h"
#include "umicom/application/production/command_binding.h"
#include "umicom/application/production/feature_binding.h"
#include "umicom/application/production/layout_binding.h"
#include "umicom/application/production/launch_plan.h"
#include "umicom/application/production/manifest_drift.h"
#include "umicom/application/production/panel_binding.h"
#include "umicom/application/production/workspace_recovery.h"

typedef struct UmiApplicationProductionRuntime {
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionPanelBindings panels;
    UmiApplicationProductionLayoutBindings layouts;
    UmiApplicationProductionFeatureBindings features;
    UmiApplicationProductionCommandBindings commands;
    UmiApplicationProductionCapabilityRequirements requirements;
    UmiApplicationProductionCapabilityProbeResults probe_results;
    UmiApplicationProductionReadinessReport readiness;
    UmiApplicationProductionManifestContract manifest_contract;
    UmiApplicationProductionManifestSnapshot manifest_snapshot;
    UmiApplicationProductionManifestDrift manifest_drift;
    UmiApplicationProductionEvidenceRequirements evidence_requirements;
    UmiApplicationProductionEvidenceRegistry evidence;
    UmiApplicationProductionAcceptanceReport acceptance;
    UmiApplicationProductionLaunchPlan launch_plan;
    UmiApplicationWorkspaceRuntime workspace;
    UmiApplicationProductionCheckpointStore checkpoints;
    UmiApplicationProductionRecoveryReport recovery;
    uint64_t revision;
    int initialised;
} UmiApplicationProductionRuntime;

UmiStatus umi_application_production_runtime_init(
    const UmiProductApplicationAdoption *adoption,
    UmiApplicationCapabilityProbe probe, void *probe_context,
    UmiApplicationProductionRuntime *out_runtime);
UmiStatus umi_application_production_runtime_refresh_acceptance(
    UmiApplicationProductionRuntime *runtime);
UmiStatus umi_application_production_runtime_checkpoint(
    UmiApplicationProductionRuntime *runtime, const char *reason,
    int clean_shutdown);

#ifdef __cplusplus
}
#endif
#endif
