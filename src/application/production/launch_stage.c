/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/launch_stage.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/launch_stage.h"

/*
 * Provide the application production launch stage text operation used by this module and
 * its client applications.
 */
const char *umi_application_production_launch_stage_text(
    UmiApplicationProductionLaunchStage stage)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (stage) {
    case UMI_APPLICATION_PRODUCTION_STAGE_RESOLVE: return "resolve";
    case UMI_APPLICATION_PRODUCTION_STAGE_VALIDATE: return "validate";
    case UMI_APPLICATION_PRODUCTION_STAGE_LOAD_LAYOUT: return "load-layout";
    case UMI_APPLICATION_PRODUCTION_STAGE_PROBE_CAPABILITIES:
        return "probe-capabilities";
    case UMI_APPLICATION_PRODUCTION_STAGE_RECOVER_WORKSPACE:
        return "recover-workspace";
    case UMI_APPLICATION_PRODUCTION_STAGE_BIND_PANELS: return "bind-panels";
    case UMI_APPLICATION_PRODUCTION_STAGE_BIND_COMMANDS: return "bind-commands";
    case UMI_APPLICATION_PRODUCTION_STAGE_ACCEPT: return "accept";
    case UMI_APPLICATION_PRODUCTION_STAGE_START: return "start";
    default: return "unknown";
    }
}

/*
 * Provide the application production state text operation used by this module and its
 * client applications.
 */
const char *umi_application_production_state_text(
    UmiApplicationProductionState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_APPLICATION_PRODUCTION_READY: return "ready";
    case UMI_APPLICATION_PRODUCTION_DEGRADED: return "degraded";
    case UMI_APPLICATION_PRODUCTION_BLOCKED: return "blocked";
    default: return "unknown";
    }
}

/*
 * Provide the application production gate text operation used by this module and its
 * client applications.
 */
const char *umi_application_production_gate_text(
    UmiApplicationProductionGate gate)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (gate) {
    case UMI_APPLICATION_PRODUCTION_GATE_CONTRACT: return "contract";
    case UMI_APPLICATION_PRODUCTION_GATE_MANIFEST: return "manifest";
    case UMI_APPLICATION_PRODUCTION_GATE_CAPABILITY: return "capability";
    case UMI_APPLICATION_PRODUCTION_GATE_RECOVERY: return "recovery";
    case UMI_APPLICATION_PRODUCTION_GATE_ACCEPTANCE: return "acceptance";
    default: return "unknown";
    }
}

