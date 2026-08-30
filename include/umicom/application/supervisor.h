/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/supervisor.h
 *
 * PURPOSE:
 *   Compose the Framework Master Controller, application definition,
 *   federation router and context hub into the only supported application
 *   runtime authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUPERVISOR_H
#define UMICOM_APPLICATION_SUPERVISOR_H

#include "umicom/application/context_hub.h"
#include "umicom/application/federation.h"
#include "umicom/application/resource_broker.h"
#include "umicom/runtime/master_controller.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationSupervisor UmiApplicationSupervisor;

typedef struct UmiApplicationSupervisorConfig {
    const char *application_id;
    UmiDiagnosticSink diagnostic_sink;
    void *diagnostic_user_data;
} UmiApplicationSupervisorConfig;

UmiStatus umi_application_supervisor_create(
    const UmiApplicationSupervisorConfig *config,
    UmiApplicationSupervisor **out_supervisor);
void umi_application_supervisor_destroy(
    UmiApplicationSupervisor *supervisor);
UmiStatus umi_application_supervisor_attach(
    UmiApplicationSupervisor *supervisor,
    const UmiApplicationDefinition *application);
UmiStatus umi_application_supervisor_start(
    UmiApplicationSupervisor *supervisor);
UmiStatus umi_application_supervisor_stop(
    UmiApplicationSupervisor *supervisor);
const UmiApplicationDefinition *umi_application_supervisor_definition(
    const UmiApplicationSupervisor *supervisor);
UmiMasterController *umi_application_supervisor_master(
    UmiApplicationSupervisor *supervisor);
UmiFederationRouter *umi_application_supervisor_federation(
    UmiApplicationSupervisor *supervisor);
UmiApplicationContextHub *umi_application_supervisor_context(
    UmiApplicationSupervisor *supervisor);
UmiApplicationResourceBroker *umi_application_supervisor_resources(
    UmiApplicationSupervisor *supervisor);

#ifdef __cplusplus
}
#endif

#endif
