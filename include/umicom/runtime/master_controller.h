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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_MASTER_CONTROLLER_H
#define UMICOM_RUNTIME_MASTER_CONTROLLER_H

#include <stddef.h>

#include "umicom/runtime/module.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the master controller data shared with callers of this public contract.
 */
typedef struct UmiMasterController UmiMasterController;

typedef void (*UmiMasterControllerAuthorityDestroyFn)(void *authority);

/**
 * Represent the master controller config data shared with callers of this public contract.
 */
typedef struct UmiMasterControllerConfig {
    const char *application_name;
    UmiDiagnosticSink diagnostic_sink;
    void *diagnostic_user_data;
} UmiMasterControllerConfig;

/**
 * Initialise master controller from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_master_controller_create(
    const UmiMasterControllerConfig *config,
    UmiMasterController **out_controller
);
/**
 * Release or reset state held by master controller so the same storage can be reused
 * safely.
 */
void umi_master_controller_destroy(UmiMasterController *controller);
/**
 * Add master controller only after its inputs and available capacity have been checked.
 */
UmiStatus umi_master_controller_register(
    UmiMasterController *controller,
    const UmiModuleDescriptor *module
);
/**
 * Provide the master controller register authority operation used by this module and its
 * client applications.
 */
UmiStatus umi_master_controller_register_authority(
    UmiMasterController *controller,
    const char *authority_id,
    void *authority,
    UmiMasterControllerAuthorityDestroyFn destroy
);
/**
 * Provide the master controller authority operation used by this module and its client
 * applications.
 */
void *umi_master_controller_authority(
    UmiMasterController *controller,
    const char *authority_id
);
/**
 * Check that master controller satisfies its contract before another service relies on it.
 */
UmiStatus umi_master_controller_validate(
    UmiMasterController *controller,
    const char **out_missing_capability
);
/**
 * Provide the master controller start operation used by this module and its client
 * applications.
 */
UmiStatus umi_master_controller_start(UmiMasterController *controller);
/**
 * Provide the master controller stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_master_controller_stop(UmiMasterController *controller);
/**
 * Provide the master controller module state operation used by this module and its client
 * applications.
 */
UmiModuleState umi_master_controller_module_state(
    const UmiMasterController *controller,
    const char *module_id
);
/**
 * Return the number of records represented by master controller module without changing
 * their state.
 */
size_t umi_master_controller_module_count(
    const UmiMasterController *controller
);
/**
 * Provide the master controller application name operation used by this module and its
 * client applications.
 */
const char *umi_master_controller_application_name(
    const UmiMasterController *controller
);
/**
 * Provide the master controller events operation used by this module and its client
 * applications.
 */
UmiEventBus *umi_master_controller_events(UmiMasterController *controller);
/**
 * Provide the master controller commands operation used by this module and its client
 * applications.
 */
UmiCommandBus *umi_master_controller_commands(UmiMasterController *controller);
/**
 * Provide the master controller queries operation used by this module and its client
 * applications.
 */
UmiQueryBus *umi_master_controller_queries(UmiMasterController *controller);
/**
 * Provide the master controller data server operation used by this module and its client
 * applications.
 */
UmiDataServer *umi_master_controller_data_server(UmiMasterController *controller);
/**
 * Provide the master controller config operation used by this module and its client
 * applications.
 */
UmiConfig *umi_master_controller_config(UmiMasterController *controller);
/**
 * Provide the master controller clock operation used by this module and its client
 * applications.
 */
UmiClock *umi_master_controller_clock(UmiMasterController *controller);
/**
 * Provide the master controller scheduler operation used by this module and its client
 * applications.
 */
UmiScheduler *umi_master_controller_scheduler(UmiMasterController *controller);
/**
 * Provide the master controller capabilities operation used by this module and its client
 * applications.
 */
UmiCapabilityRegistry *umi_master_controller_capabilities(
    UmiMasterController *controller
);
/**
 * Provide the master controller services operation used by this module and its client
 * applications.
 */
UmiServiceRegistry *umi_master_controller_services(
    UmiMasterController *controller
);
/**
 * Provide the master controller command registry operation used by this module and its
 * client applications.
 */
UmiCommandRegistry *umi_master_controller_command_registry(
    UmiMasterController *controller
);
/**
 * Provide the master controller health operation used by this module and its client
 * applications.
 */
UmiHealthRegistry *umi_master_controller_health(
    UmiMasterController *controller
);
/**
 * Provide the master controller policy operation used by this module and its client
 * applications.
 */
UmiPolicyEngine *umi_master_controller_policy(UmiMasterController *controller);

#ifdef __cplusplus
}
#endif

#endif
