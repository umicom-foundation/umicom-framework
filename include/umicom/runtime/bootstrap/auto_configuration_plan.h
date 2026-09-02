/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/auto_configuration_plan.h
 *
 * PURPOSE:
 *   Select and priority-order applicable auto-configurations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_PLAN_H
#define UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_PLAN_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap auto configuration plan from caller-provided values so later
 * operations receive a known state.
 */
void umi_bootstrap_auto_configuration_plan_init(
    UmiBootstrapAutoConfigurationPlan *plan);
/**
 * Provide the bootstrap auto configuration plan build operation used by this module and
 * its client applications.
 */
UmiStatus umi_bootstrap_auto_configuration_plan_build(
    const UmiBootstrapAutoConfigurationCatalogue *catalogue,
    const UmiBootstrapPropertySet *properties,
    const UmiBootstrapPropertySet *environment,
    const UmiBootstrapIdList *features,
    const UmiBootstrapIdList *capabilities,
    const char *platform_id,
    UmiBootstrapAutoConfigurationPlan *out_plan);

#ifdef __cplusplus
}
#endif

#endif
