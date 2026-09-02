/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/launch_stage.h
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
#ifndef UMICOM_APPLICATION_PRODUCTION_LAUNCH_STAGE_H
#define UMICOM_APPLICATION_PRODUCTION_LAUNCH_STAGE_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/types.h"

/**
 * Provide the application production launch stage text operation used by this module and
 * its client applications.
 */
const char *umi_application_production_launch_stage_text(
    UmiApplicationProductionLaunchStage stage);
/**
 * Provide the application production state text operation used by this module and its
 * client applications.
 */
const char *umi_application_production_state_text(
    UmiApplicationProductionState state);
/**
 * Provide the application production gate text operation used by this module and its
 * client applications.
 */
const char *umi_application_production_gate_text(
    UmiApplicationProductionGate gate);

#ifdef __cplusplus
}
#endif
#endif
