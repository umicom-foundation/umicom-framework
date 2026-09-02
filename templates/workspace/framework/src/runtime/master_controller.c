/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/runtime/master_controller.c
 *
 * PURPOSE:
 *   Implement the master controller behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/master_controller.h"
#include "umicom/runtime/module_registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_MASTER_MAX_MODULES 128U

typedef struct UmiModuleRuntimeEntry {
    const UmiModuleDescriptor *descriptor;
    UmiModuleContext context;
    UmiModuleState state;
} UmiModuleRuntimeEntry;

struct UmiMasterController {
    char application_name[128];
    UmiDiagnosticSink diagnostic_sink;
    void *diagnostic_user_data;
    UmiEventBus *events;
    UmiCommandBus *commands;
    UmiQueryBus *queries;
    UmiDataServer *data_server;
    UmiConfig *config;
    UmiClock clock;
    UmiScheduler *scheduler;
    UmiModuleRegistry *registry;
    UmiModuleRuntimeEntry modules[UMI_MASTER_MAX_MODULES];
    size_t module_count;
    int started;
};

/* Provide the report operation used by this module and its client applications. */
static void report(UmiMasterController *controller, UmiDiagnosticSeverity severity,
                   const char *message)
{
    umi_diagnostic_emit(controller->diagnostic_sink, controller->diagnostic_user_data,
                        severity, "master-controller", message, 0U);
}

/*
 * Initialise master controller from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_master_controller_create(const UmiMasterControllerConfig *config,
                                        UmiMasterController **out_controller)
{
    UmiMasterController *controller;
    /* Apply this branch only when its contract condition is satisfied. */
    if (config == 0 || config->application_name == 0 || out_controller == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_controller = 0;
    controller = calloc(1U, sizeof(*controller));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (controller == 0) return UMI_STATUS_OUT_OF_MEMORY;
    (void)snprintf(controller->application_name, sizeof(controller->application_name),
                   "%s", config->application_name);
    controller->diagnostic_sink = config->diagnostic_sink;
    controller->diagnostic_user_data = config->diagnostic_user_data;
    controller->clock = umi_clock_system();
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_event_bus_create(&controller->events) != UMI_STATUS_OK ||
        umi_command_bus_create(&controller->commands) != UMI_STATUS_OK ||
        umi_query_bus_create(&controller->queries) != UMI_STATUS_OK ||
        umi_data_server_create_memory(&controller->data_server) != UMI_STATUS_OK ||
        umi_config_create(&controller->config) != UMI_STATUS_OK ||
        umi_scheduler_create(&controller->clock, &controller->scheduler) != UMI_STATUS_OK ||
        umi_module_registry_create(&controller->registry) != UMI_STATUS_OK) {
        umi_master_controller_destroy(controller);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *out_controller = controller;
    report(controller, UMI_DIAGNOSTIC_INFO, "Master Controller created");
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by master controller so the same storage can be reused
 * safely.
 */
void umi_master_controller_destroy(UmiMasterController *controller)
{
    size_t i;
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller == 0) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->started) (void)umi_master_controller_stop(controller);
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = controller->module_count; i > 0U; --i) {
        UmiModuleRuntimeEntry *entry = &controller->modules[i - 1U];
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->state != UMI_MODULE_DESTROYED && entry->descriptor->lifecycle.destroy != 0)
            entry->descriptor->lifecycle.destroy(&entry->context);
        entry->state = UMI_MODULE_DESTROYED;
    }
    umi_module_registry_destroy(controller->registry);
    umi_scheduler_destroy(controller->scheduler);
    umi_clock_dispose(&controller->clock);
    umi_config_destroy(controller->config);
    umi_data_server_destroy(controller->data_server);
    umi_query_bus_destroy(controller->queries);
    umi_command_bus_destroy(controller->commands);
    umi_event_bus_destroy(controller->events);
    free(controller);
}

/* Add master controller only after its inputs and available capacity have been checked. */
UmiStatus umi_master_controller_register(UmiMasterController *controller,
                                         const UmiModuleDescriptor *module)
{
    UmiModuleRuntimeEntry *entry;
    UmiStatus status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (controller == 0 || module == 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (controller->started) return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (controller->module_count >= UMI_MASTER_MAX_MODULES) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_module_registry_add(controller->registry, module);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    entry = &controller->modules[controller->module_count++];
    memset(entry, 0, sizeof(*entry));
    entry->descriptor = module;
    entry->state = UMI_MODULE_DISCOVERED;
    entry->context.module_state = module->module_state;
    entry->context.events = controller->events;
    entry->context.commands = controller->commands;
    entry->context.queries = controller->queries;
    entry->context.data_server = controller->data_server;
    entry->context.config = controller->config;
    entry->context.clock = &controller->clock;
    entry->context.scheduler = controller->scheduler;
    entry->context.diagnostic_sink = controller->diagnostic_sink;
    entry->context.diagnostic_user_data = controller->diagnostic_user_data;
    report(controller, UMI_DIAGNOSTIC_INFO, "Slave Controller registered");
    return UMI_STATUS_OK;
}

/* Provide the run phase operation used by this module and its client applications. */
static UmiStatus run_phase(UmiMasterController *controller, UmiModuleRuntimeEntry *entry,
                           UmiModulePhaseFn phase, UmiModuleState success_state,
                           const char *failure_message)
{
    UmiStatus status = UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (phase != 0) status = phase(&entry->context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        entry->state = UMI_MODULE_FAILED;
        report(controller, UMI_DIAGNOSTIC_ERROR, failure_message);
        return status;
    }
    entry->state = success_state;
    return UMI_STATUS_OK;
}

/*
 * Provide the master controller start operation used by this module and its client
 * applications.
 */
UmiStatus umi_master_controller_start(UmiMasterController *controller)
{
    size_t i;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (controller == 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (controller->started) return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < controller->module_count; ++i) {
        UmiModuleRuntimeEntry *entry = &controller->modules[i];
        UmiStatus status = run_phase(controller, entry, entry->descriptor->lifecycle.configure,
                                     UMI_MODULE_CONFIGURED, "Module configure failed");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = run_phase(controller, entry, entry->descriptor->lifecycle.initialise,
                           UMI_MODULE_INITIALISED, "Module initialise failed");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = run_phase(controller, entry, entry->descriptor->lifecycle.start,
                           UMI_MODULE_STARTED, "Module start failed");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    controller->started = 1;
    report(controller, UMI_DIAGNOSTIC_INFO, "All Slave Controllers started");
    return UMI_STATUS_OK;
}

/*
 * Provide the master controller stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_master_controller_stop(UmiMasterController *controller)
{
    size_t i;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (controller == 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!controller->started) return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = controller->module_count; i > 0U; --i) {
        UmiModuleRuntimeEntry *entry = &controller->modules[i - 1U];
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->descriptor->lifecycle.quiesce != 0) {
            UmiStatus status = entry->descriptor->lifecycle.quiesce(&entry->context);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) { entry->state = UMI_MODULE_FAILED; return status; }
            entry->state = UMI_MODULE_QUIESCED;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->descriptor->lifecycle.stop != 0) {
            UmiStatus status = entry->descriptor->lifecycle.stop(&entry->context);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) { entry->state = UMI_MODULE_FAILED; return status; }
        }
        entry->state = UMI_MODULE_STOPPED;
    }
    controller->started = 0;
    report(controller, UMI_DIAGNOSTIC_INFO, "All Slave Controllers stopped");
    return UMI_STATUS_OK;
}

/*
 * Provide the master controller module state operation used by this module and its client
 * applications.
 */
UmiModuleState umi_master_controller_module_state(const UmiMasterController *controller,
                                                  const char *module_id)
{
    size_t i;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (controller == 0 || module_id == 0) return UMI_MODULE_FAILED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < controller->module_count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(controller->modules[i].descriptor->module_id, module_id) == 0)
            return controller->modules[i].state;
    return UMI_MODULE_FAILED;
}
/*
 * Return the number of records represented by master controller module without changing
 * their state.
 */
size_t umi_master_controller_module_count(const UmiMasterController *controller)
{ return controller != 0 ? controller->module_count : 0U; }
/*
 * Provide the master controller application name operation used by this module and its
 * client applications.
 */
const char *umi_master_controller_application_name(const UmiMasterController *controller)
{ return controller != 0 ? controller->application_name : ""; }
/*
 * Provide the master controller events operation used by this module and its client
 * applications.
 */
UmiEventBus *umi_master_controller_events(UmiMasterController *controller)
{ return controller != 0 ? controller->events : 0; }
/*
 * Provide the master controller commands operation used by this module and its client
 * applications.
 */
UmiCommandBus *umi_master_controller_commands(UmiMasterController *controller)
{ return controller != 0 ? controller->commands : 0; }
/*
 * Provide the master controller queries operation used by this module and its client
 * applications.
 */
UmiQueryBus *umi_master_controller_queries(UmiMasterController *controller)
{ return controller != 0 ? controller->queries : 0; }
/*
 * Provide the master controller data server operation used by this module and its client
 * applications.
 */
UmiDataServer *umi_master_controller_data_server(UmiMasterController *controller)
{ return controller != 0 ? controller->data_server : 0; }
/*
 * Provide the master controller config operation used by this module and its client
 * applications.
 */
UmiConfig *umi_master_controller_config(UmiMasterController *controller)
{ return controller != 0 ? controller->config : 0; }

/*
 * Provide the master controller clock operation used by this module and its client
 * applications.
 */
UmiClock *umi_master_controller_clock(UmiMasterController *controller)
{ return controller != 0 ? &controller->clock : 0; }
/*
 * Provide the master controller scheduler operation used by this module and its client
 * applications.
 */
UmiScheduler *umi_master_controller_scheduler(UmiMasterController *controller)
{ return controller != 0 ? controller->scheduler : 0; }
