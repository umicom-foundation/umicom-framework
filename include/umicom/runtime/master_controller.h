/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/master_controller.h
 *
 * PURPOSE:
 *   Define the Framework Master Controller composition root.  It owns shared
 *   services, command metadata, health, capability and policy registries,
 *   validates module dependencies, starts Slave Controllers in dependency
 *   order, rolls back failed startup and stops modules in reverse order.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_MASTER_CONTROLLER_H
#define UMICOM_RUNTIME_MASTER_CONTROLLER_H

#include <stddef.h>

#include "umicom/runtime/module.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiMasterController UmiMasterController;

typedef struct UmiMasterControllerConfig {
    const char *application_name;
    UmiDiagnosticSink diagnostic_sink;
    void *diagnostic_user_data;
} UmiMasterControllerConfig;

UmiStatus umi_master_controller_create(
    const UmiMasterControllerConfig *config,
    UmiMasterController **out_controller
);
void umi_master_controller_destroy(UmiMasterController *controller);
UmiStatus umi_master_controller_register(
    UmiMasterController *controller,
    const UmiModuleDescriptor *module
);
UmiStatus umi_master_controller_validate(
    UmiMasterController *controller,
    const char **out_missing_capability
);
UmiStatus umi_master_controller_start(UmiMasterController *controller);
UmiStatus umi_master_controller_stop(UmiMasterController *controller);
UmiModuleState umi_master_controller_module_state(
    const UmiMasterController *controller,
    const char *module_id
);
size_t umi_master_controller_module_count(
    const UmiMasterController *controller
);
const char *umi_master_controller_application_name(
    const UmiMasterController *controller
);
UmiEventBus *umi_master_controller_events(UmiMasterController *controller);
UmiCommandBus *umi_master_controller_commands(UmiMasterController *controller);
UmiQueryBus *umi_master_controller_queries(UmiMasterController *controller);
UmiDataServer *umi_master_controller_data_server(UmiMasterController *controller);
UmiConfig *umi_master_controller_config(UmiMasterController *controller);
UmiClock *umi_master_controller_clock(UmiMasterController *controller);
UmiScheduler *umi_master_controller_scheduler(UmiMasterController *controller);
UmiCapabilityRegistry *umi_master_controller_capabilities(
    UmiMasterController *controller
);
UmiServiceRegistry *umi_master_controller_services(
    UmiMasterController *controller
);
UmiCommandRegistry *umi_master_controller_command_registry(
    UmiMasterController *controller
);
UmiHealthRegistry *umi_master_controller_health(
    UmiMasterController *controller
);
UmiPolicyEngine *umi_master_controller_policy(UmiMasterController *controller);

#ifdef __cplusplus
}
#endif

#endif
