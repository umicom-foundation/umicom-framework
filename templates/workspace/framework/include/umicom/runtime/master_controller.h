/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/runtime/master_controller.h
 *
 * PURPOSE:
 *   Declare the master controller contract shared by Framework services and thin
 *   applications.
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
UmiStatus umi_master_controller_create(const UmiMasterControllerConfig *config,
                                        UmiMasterController **out_controller);
/**
 * Release or reset state held by master controller so the same storage can be reused
 * safely.
 */
void umi_master_controller_destroy(UmiMasterController *controller);
/**
 * Add master controller only after its inputs and available capacity have been checked.
 */
UmiStatus umi_master_controller_register(UmiMasterController *controller,
                                         const UmiModuleDescriptor *module);
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
UmiModuleState umi_master_controller_module_state(const UmiMasterController *controller,
                                                  const char *module_id);
/**
 * Return the number of records represented by master controller module without changing
 * their state.
 */
size_t umi_master_controller_module_count(const UmiMasterController *controller);
/**
 * Provide the master controller application name operation used by this module and its
 * client applications.
 */
const char *umi_master_controller_application_name(const UmiMasterController *controller);
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

#ifdef __cplusplus
}
#endif

#endif
