/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/auto_configuration_plan.h
 *
 * PURPOSE:
 *   Select and priority-order applicable auto-configurations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_PLAN_H
#define UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_PLAN_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_auto_configuration_plan_init(
    UmiBootstrapAutoConfigurationPlan *plan);
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
