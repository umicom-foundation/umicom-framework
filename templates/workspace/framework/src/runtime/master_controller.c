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

static void report(UmiMasterController *controller, UmiDiagnosticSeverity severity,
                   const char *message)
{
    umi_diagnostic_emit(controller->diagnostic_sink, controller->diagnostic_user_data,
                        severity, "master-controller", message, 0U);
}

UmiStatus umi_master_controller_create(const UmiMasterControllerConfig *config,
                                        UmiMasterController **out_controller)
{
    UmiMasterController *controller;
    if (config == 0 || config->application_name == 0 || out_controller == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_controller = 0;
    controller = calloc(1U, sizeof(*controller));
    if (controller == 0) return UMI_STATUS_OUT_OF_MEMORY;
    (void)snprintf(controller->application_name, sizeof(controller->application_name),
                   "%s", config->application_name);
    controller->diagnostic_sink = config->diagnostic_sink;
    controller->diagnostic_user_data = config->diagnostic_user_data;
    controller->clock = umi_clock_system();
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

void umi_master_controller_destroy(UmiMasterController *controller)
{
    size_t i;
    if (controller == 0) return;
    if (controller->started) (void)umi_master_controller_stop(controller);
    for (i = controller->module_count; i > 0U; --i) {
        UmiModuleRuntimeEntry *entry = &controller->modules[i - 1U];
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

UmiStatus umi_master_controller_register(UmiMasterController *controller,
                                         const UmiModuleDescriptor *module)
{
    UmiModuleRuntimeEntry *entry;
    UmiStatus status;
    if (controller == 0 || module == 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->started) return UMI_STATUS_INVALID_STATE;
    if (controller->module_count >= UMI_MASTER_MAX_MODULES) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_module_registry_add(controller->registry, module);
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

static UmiStatus run_phase(UmiMasterController *controller, UmiModuleRuntimeEntry *entry,
                           UmiModulePhaseFn phase, UmiModuleState success_state,
                           const char *failure_message)
{
    UmiStatus status = UMI_STATUS_OK;
    if (phase != 0) status = phase(&entry->context);
    if (status != UMI_STATUS_OK) {
        entry->state = UMI_MODULE_FAILED;
        report(controller, UMI_DIAGNOSTIC_ERROR, failure_message);
        return status;
    }
    entry->state = success_state;
    return UMI_STATUS_OK;
}

UmiStatus umi_master_controller_start(UmiMasterController *controller)
{
    size_t i;
    if (controller == 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->started) return UMI_STATUS_INVALID_STATE;
    for (i = 0U; i < controller->module_count; ++i) {
        UmiModuleRuntimeEntry *entry = &controller->modules[i];
        UmiStatus status = run_phase(controller, entry, entry->descriptor->lifecycle.configure,
                                     UMI_MODULE_CONFIGURED, "Module configure failed");
        if (status != UMI_STATUS_OK) return status;
        status = run_phase(controller, entry, entry->descriptor->lifecycle.initialise,
                           UMI_MODULE_INITIALISED, "Module initialise failed");
        if (status != UMI_STATUS_OK) return status;
        status = run_phase(controller, entry, entry->descriptor->lifecycle.start,
                           UMI_MODULE_STARTED, "Module start failed");
        if (status != UMI_STATUS_OK) return status;
    }
    controller->started = 1;
    report(controller, UMI_DIAGNOSTIC_INFO, "All Slave Controllers started");
    return UMI_STATUS_OK;
}

UmiStatus umi_master_controller_stop(UmiMasterController *controller)
{
    size_t i;
    if (controller == 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (!controller->started) return UMI_STATUS_INVALID_STATE;
    for (i = controller->module_count; i > 0U; --i) {
        UmiModuleRuntimeEntry *entry = &controller->modules[i - 1U];
        if (entry->descriptor->lifecycle.quiesce != 0) {
            UmiStatus status = entry->descriptor->lifecycle.quiesce(&entry->context);
            if (status != UMI_STATUS_OK) { entry->state = UMI_MODULE_FAILED; return status; }
            entry->state = UMI_MODULE_QUIESCED;
        }
        if (entry->descriptor->lifecycle.stop != 0) {
            UmiStatus status = entry->descriptor->lifecycle.stop(&entry->context);
            if (status != UMI_STATUS_OK) { entry->state = UMI_MODULE_FAILED; return status; }
        }
        entry->state = UMI_MODULE_STOPPED;
    }
    controller->started = 0;
    report(controller, UMI_DIAGNOSTIC_INFO, "All Slave Controllers stopped");
    return UMI_STATUS_OK;
}

UmiModuleState umi_master_controller_module_state(const UmiMasterController *controller,
                                                  const char *module_id)
{
    size_t i;
    if (controller == 0 || module_id == 0) return UMI_MODULE_FAILED;
    for (i = 0U; i < controller->module_count; ++i)
        if (strcmp(controller->modules[i].descriptor->module_id, module_id) == 0)
            return controller->modules[i].state;
    return UMI_MODULE_FAILED;
}
size_t umi_master_controller_module_count(const UmiMasterController *controller)
{ return controller != 0 ? controller->module_count : 0U; }
const char *umi_master_controller_application_name(const UmiMasterController *controller)
{ return controller != 0 ? controller->application_name : ""; }
UmiEventBus *umi_master_controller_events(UmiMasterController *controller)
{ return controller != 0 ? controller->events : 0; }
UmiCommandBus *umi_master_controller_commands(UmiMasterController *controller)
{ return controller != 0 ? controller->commands : 0; }
UmiQueryBus *umi_master_controller_queries(UmiMasterController *controller)
{ return controller != 0 ? controller->queries : 0; }
UmiDataServer *umi_master_controller_data_server(UmiMasterController *controller)
{ return controller != 0 ? controller->data_server : 0; }
UmiConfig *umi_master_controller_config(UmiMasterController *controller)
{ return controller != 0 ? controller->config : 0; }

UmiClock *umi_master_controller_clock(UmiMasterController *controller)
{ return controller != 0 ? &controller->clock : 0; }
UmiScheduler *umi_master_controller_scheduler(UmiMasterController *controller)
{ return controller != 0 ? controller->scheduler : 0; }
