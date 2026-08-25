/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/bootstrap_service.h
 *
 * PURPOSE:
 *   Coordinate graph validation, starter and auto-configuration planning for application launch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_SERVICE_H
#define UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_SERVICE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_service_prepare(
    UmiBootstrapContext *context,
    const UmiBootstrapServiceGraph *service_graph,
    const UmiBootstrapStarterCatalogue *starters,
    const UmiBootstrapAutoConfigurationCatalogue *auto_configurations,
    const UmiBootstrapPropertySet *environment,
    const UmiBootstrapIdList *features,
    const UmiBootstrapIdList *capabilities,
    UmiBootstrapPlan *out_bootstrap_plan,
    UmiBootstrapAutoConfigurationPlan *out_auto_configuration_plan,
    UmiBootstrapIssueReport *out_issues);

#ifdef __cplusplus
}
#endif

#endif
