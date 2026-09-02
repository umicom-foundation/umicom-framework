/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/bootstrap_service.h
 *
 * PURPOSE:
 *   Coordinate graph validation, starter and auto-configuration planning for application launch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_SERVICE_H
#define UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_SERVICE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap service prepare operation used by this module and its client
 * applications.
 */
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
