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

/**
 * Represent the application supervisor data shared with callers of this public contract.
 */
typedef struct UmiApplicationSupervisor UmiApplicationSupervisor;

/**
 * Represent the application supervisor config data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationSupervisorConfig {
    const char *application_id;
    UmiDiagnosticSink diagnostic_sink;
    void *diagnostic_user_data;
} UmiApplicationSupervisorConfig;

/**
 * Initialise application supervisor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_supervisor_create(
    const UmiApplicationSupervisorConfig *config,
    UmiApplicationSupervisor **out_supervisor);
/**
 * Release or reset state held by application supervisor so the same storage can be reused
 * safely.
 */
void umi_application_supervisor_destroy(
    UmiApplicationSupervisor *supervisor);
/**
 * Provide the application supervisor attach operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_supervisor_attach(
    UmiApplicationSupervisor *supervisor,
    const UmiApplicationDefinition *application);
/**
 * Provide the application supervisor start operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_supervisor_start(
    UmiApplicationSupervisor *supervisor);
/**
 * Provide the application supervisor stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_supervisor_stop(
    UmiApplicationSupervisor *supervisor);
/**
 * Provide the application supervisor definition operation used by this module and its
 * client applications.
 */
const UmiApplicationDefinition *umi_application_supervisor_definition(
    const UmiApplicationSupervisor *supervisor);
/**
 * Provide the application supervisor master operation used by this module and its client
 * applications.
 */
UmiMasterController *umi_application_supervisor_master(
    UmiApplicationSupervisor *supervisor);
/**
 * Provide the application supervisor federation operation used by this module and its
 * client applications.
 */
UmiFederationRouter *umi_application_supervisor_federation(
    UmiApplicationSupervisor *supervisor);
/**
 * Provide the application supervisor context operation used by this module and its client
 * applications.
 */
UmiApplicationContextHub *umi_application_supervisor_context(
    UmiApplicationSupervisor *supervisor);
/**
 * Provide the application supervisor resources operation used by this module and its
 * client applications.
 */
UmiApplicationResourceBroker *umi_application_supervisor_resources(
    UmiApplicationSupervisor *supervisor);

#ifdef __cplusplus
}
#endif

#endif
