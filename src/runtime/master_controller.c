/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/master_controller.c
 *
 * PURPOSE:
 *   Implement the Master Controller, shared services, explicit service and
 *   command registries, health state, capability registration, deterministic
 *   Slave Controller startup, failed-start rollback and reverse shutdown.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/master_controller.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/runtime/module_registry.h"

#define UMI_MASTER_MAX_MODULES 128U
#define UMI_MASTER_MAX_START_ORDER 128U
#define UMI_MASTER_MAX_AUTHORITIES 32U
#define UMI_MASTER_AUTHORITY_ID_CAPACITY 128U

typedef struct UmiModuleRuntimeEntry {
    const UmiModuleDescriptor *descriptor;
    UmiModuleContext context;
    UmiModuleState state;
} UmiModuleRuntimeEntry;

typedef struct UmiMasterAuthorityEntry {
    char authority_id[UMI_MASTER_AUTHORITY_ID_CAPACITY];
    void *authority;
    UmiMasterControllerAuthorityDestroyFn destroy;
} UmiMasterAuthorityEntry;

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
    UmiCapabilityRegistry *capabilities;
    UmiServiceRegistry *services;
    UmiCommandRegistry *command_registry;
    UmiHealthRegistry *health;
    UmiPolicyEngine *policy;
    UmiModuleRuntimeEntry modules[UMI_MASTER_MAX_MODULES];
    UmiMasterAuthorityEntry authorities[UMI_MASTER_MAX_AUTHORITIES];
    size_t start_order[UMI_MASTER_MAX_START_ORDER];
    size_t start_order_count;
    size_t module_count;
    size_t authority_count;
    int started;
};

/* Provide the master now operation used by this module and its client applications. */
static uint64_t umi_master_now(const UmiMasterController *controller)
{
    return controller != NULL &&
           controller->clock.wall_nanoseconds != NULL
        ? controller->clock.wall_nanoseconds(&controller->clock)
        : 0U;
}

/* Provide the master report operation used by this module and its client applications. */
static void umi_master_report(UmiMasterController *controller,
                              UmiDiagnosticSeverity severity,
                              const char *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return;
    }
    umi_diagnostic_emit(controller->diagnostic_sink,
                        controller->diagnostic_user_data,
                        severity,
                        "master-controller",
                        message,
                        0U);
}

/* Provide the master health operation used by this module and its client applications. */
static void umi_master_health(UmiMasterController *controller,
                              const char *component_id,
                              UmiHealthState state,
                              const char *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || controller->health == NULL) {
        return;
    }
    (void)umi_health_registry_update(controller->health,
                                     component_id,
                                     state,
                                     message,
                                     umi_master_now(controller));
}

/*
 * Provide the master register core service operation used by this module and its client
 * applications.
 */
static UmiStatus umi_master_register_core_service(
    UmiMasterController *controller,
    const char *service_id,
    void *service,
    uint32_t flags)
{
    UmiServiceDescriptor descriptor;

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.service_id = service_id;
    descriptor.service_version = (UmiVersion){0U, 5U, 0U};
    descriptor.provider_module_id = "org.umicom.framework.core";
    descriptor.service = service;
    descriptor.flags = flags;
    return umi_service_registry_register(controller->services, &descriptor);
}

/*
 * Provide the master register core capability operation used by this module and its client
 * applications.
 */
static UmiStatus umi_master_register_core_capability(
    UmiMasterController *controller,
    const char *capability_id,
    void *provider)
{
    UmiCapabilityDescriptor descriptor;

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.capability_id = capability_id;
    descriptor.capability_version = (UmiVersion){0U, 5U, 0U};
    descriptor.provider_module_id = "org.umicom.framework.core";
    descriptor.provider = provider;
    descriptor.flags = UMI_CAPABILITY_SINGLETON | UMI_CAPABILITY_EXTERNAL;
    return umi_capability_registry_register(controller->capabilities,
                                             &descriptor);
}

/*
 * Provide the master register core capabilities operation used by this module and its
 * client applications.
 */
static UmiStatus umi_master_register_core_capabilities(
    UmiMasterController *controller)
{
    UmiStatus status;

#define UMI_REGISTER_CORE(id, provider)                                     \
    do {                                                                    \
        status = umi_master_register_core_capability(controller,            \
                                                     (id),                  \
                                                     (provider));           \
        if (status != UMI_STATUS_OK) return status;                         \
    } while (0)

    UMI_REGISTER_CORE("umicom.diagnostics", controller->diagnostic_user_data);
    UMI_REGISTER_CORE("umicom.messaging.events", controller->events);
    UMI_REGISTER_CORE("umicom.messaging.commands", controller->commands);
    UMI_REGISTER_CORE("umicom.messaging.queries", controller->queries);
    UMI_REGISTER_CORE("umicom.data", controller->data_server);
    UMI_REGISTER_CORE("umicom.configuration", controller->config);
    UMI_REGISTER_CORE("umicom.clock", &controller->clock);
    UMI_REGISTER_CORE("umicom.scheduler", controller->scheduler);
    UMI_REGISTER_CORE("umicom.security.policy", controller->policy);
    UMI_REGISTER_CORE("umicom.runtime.capabilities", controller->capabilities);
    UMI_REGISTER_CORE("umicom.runtime.services", controller->services);
    UMI_REGISTER_CORE("umicom.runtime.commands", controller->command_registry);
    UMI_REGISTER_CORE("umicom.runtime.health", controller->health);
    UMI_REGISTER_CORE("umicom.runtime.modules", controller->registry);
    UMI_REGISTER_CORE("umicom.filesystem", controller);
    UMI_REGISTER_CORE("umicom.process", controller);
#undef UMI_REGISTER_CORE

#define UMI_REGISTER_SERVICE(id, service, flags)                            \
    do {                                                                    \
        status = umi_master_register_core_service(controller,               \
                                                  (id),                     \
                                                  (service),                \
                                                  (flags));                 \
        if (status != UMI_STATUS_OK) return status;                         \
    } while (0)

    UMI_REGISTER_SERVICE("umicom.messaging.events",
                         controller->events,
                         UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    UMI_REGISTER_SERVICE("umicom.messaging.commands",
                         controller->commands,
                         UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    UMI_REGISTER_SERVICE("umicom.messaging.queries",
                         controller->queries,
                         UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    UMI_REGISTER_SERVICE("umicom.data",
                         controller->data_server,
                         UMI_SERVICE_SINGLETON);
    UMI_REGISTER_SERVICE("umicom.configuration",
                         controller->config,
                         UMI_SERVICE_SINGLETON);
    UMI_REGISTER_SERVICE("umicom.clock",
                         &controller->clock,
                         UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    UMI_REGISTER_SERVICE("umicom.scheduler",
                         controller->scheduler,
                         UMI_SERVICE_SINGLETON);
    UMI_REGISTER_SERVICE("umicom.security.policy",
                         controller->policy,
                         UMI_SERVICE_SINGLETON);
    UMI_REGISTER_SERVICE("umicom.runtime.capabilities",
                         controller->capabilities,
                         UMI_SERVICE_SINGLETON);
    UMI_REGISTER_SERVICE("umicom.runtime.commands",
                         controller->command_registry,
                         UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    UMI_REGISTER_SERVICE("umicom.runtime.health",
                         controller->health,
                         UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    UMI_REGISTER_SERVICE("umicom.runtime.modules",
                         controller->registry,
                         UMI_SERVICE_SINGLETON);
#undef UMI_REGISTER_SERVICE

    return UMI_STATUS_OK;
}

/*
 * Initialise master controller from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_master_controller_create(
    const UmiMasterControllerConfig *config,
    UmiMasterController **out_controller)
{
    UmiMasterController *controller;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || config->application_name == NULL ||
        config->application_name[0] == '\0' || out_controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_controller = NULL;

    controller = (UmiMasterController *)calloc(1U, sizeof(*controller));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    (void)snprintf(controller->application_name,
                   sizeof(controller->application_name),
                   "%s",
                   config->application_name);
    controller->diagnostic_sink = config->diagnostic_sink;
    controller->diagnostic_user_data = config->diagnostic_user_data;
    controller->clock = umi_clock_system();

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_event_bus_create(&controller->events) != UMI_STATUS_OK ||
        umi_command_bus_create(&controller->commands) != UMI_STATUS_OK ||
        umi_query_bus_create(&controller->queries) != UMI_STATUS_OK ||
        umi_data_server_create_memory(&controller->data_server) != UMI_STATUS_OK ||
        umi_config_create(&controller->config) != UMI_STATUS_OK ||
        umi_scheduler_create(&controller->clock,
                             &controller->scheduler) != UMI_STATUS_OK ||
        umi_module_registry_create(&controller->registry) != UMI_STATUS_OK ||
        umi_capability_registry_create(&controller->capabilities) !=
            UMI_STATUS_OK ||
        umi_service_registry_create(&controller->services) != UMI_STATUS_OK ||
        umi_command_registry_create(&controller->command_registry) !=
            UMI_STATUS_OK ||
        umi_health_registry_create(&controller->health) != UMI_STATUS_OK ||
        umi_policy_engine_create(&controller->policy) != UMI_STATUS_OK) {
        umi_master_controller_destroy(controller);
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    status = umi_master_register_core_capabilities(controller);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_master_controller_destroy(controller);
        return status;
    }

    umi_master_health(controller,
                      "org.umicom.framework.master",
                      UMI_HEALTH_READY,
                      "Master Controller created");
    *out_controller = controller;
    umi_master_report(controller,
                      UMI_DIAGNOSTIC_INFO,
                      "Master Controller created");
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by master controller so the same storage can be reused
 * safely.
 */
void umi_master_controller_destroy(UmiMasterController *controller)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->started) {
        (void)umi_master_controller_stop(controller);
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = controller->module_count; index > 0U; --index) {
        UmiModuleRuntimeEntry *entry = &controller->modules[index - 1U];
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->state != UMI_MODULE_DESTROYED &&
            entry->descriptor != NULL &&
            entry->descriptor->lifecycle.destroy != NULL) {
            entry->descriptor->lifecycle.destroy(&entry->context);
        }
        entry->state = UMI_MODULE_DESTROYED;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (entry->descriptor != NULL) {
            umi_master_health(controller,
                              entry->descriptor->module_id,
                              UMI_HEALTH_STOPPED,
                              "Module destroyed");
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = controller->authority_count; index > 0U; --index) {
        UmiMasterAuthorityEntry *entry = &controller->authorities[index - 1U];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (entry->destroy != NULL) {
            entry->destroy(entry->authority);
        }
        entry->authority = NULL;
        entry->destroy = NULL;
    }

    umi_service_registry_destroy(controller->services);
    umi_command_registry_destroy(controller->command_registry);
    umi_health_registry_destroy(controller->health);
    umi_policy_engine_destroy(controller->policy);
    umi_capability_registry_destroy(controller->capabilities);
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

/*
 * Provide the master controller register authority operation used by this module and its
 * client applications.
 */
UmiStatus umi_master_controller_register_authority(
    UmiMasterController *controller,
    const char *authority_id,
    void *authority,
    UmiMasterControllerAuthorityDestroyFn destroy)
{
    UmiMasterAuthorityEntry *entry;
    size_t index;
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || authority_id == NULL ||
        authority_id[0] == '\0' || authority == NULL || destroy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->started) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < controller->authority_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(controller->authorities[index].authority_id,
                   authority_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (controller->authority_count >= UMI_MASTER_MAX_AUTHORITIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    entry = &controller->authorities[controller->authority_count];
    written = snprintf(entry->authority_id, sizeof(entry->authority_id),
                       "%s", authority_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(entry->authority_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry->authority = authority;
    entry->destroy = destroy;
    controller->authority_count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the master controller authority operation used by this module and its client
 * applications.
 */
void *umi_master_controller_authority(
    UmiMasterController *controller,
    const char *authority_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || authority_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < controller->authority_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(controller->authorities[index].authority_id,
                   authority_id) == 0) {
            return controller->authorities[index].authority;
        }
    }
    return NULL;
}

/*
 * Provide the master register module capabilities operation used by this module and its
 * client applications.
 */
static UmiStatus umi_master_register_module_capabilities(
    UmiMasterController *controller,
    const UmiModuleDescriptor *module)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (module->provided_capabilities == NULL) {
        return UMI_STATUS_OK;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; module->provided_capabilities[index] != NULL; ++index) {
        UmiCapabilityDescriptor descriptor;
        UmiStatus status;

        (void)memset(&descriptor, 0, sizeof(descriptor));
        descriptor.structure_size = (uint32_t)sizeof(descriptor);
        descriptor.capability_id = module->provided_capabilities[index];
        descriptor.capability_version = module->module_version;
        descriptor.provider_module_id = module->module_id;
        descriptor.provider = module->module_state;
        descriptor.flags = UMI_CAPABILITY_SINGLETON;
        status = umi_capability_registry_register(controller->capabilities,
                                                   &descriptor);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

/* Add master controller only after its inputs and available capacity have been checked. */
UmiStatus umi_master_controller_register(
    UmiMasterController *controller,
    const UmiModuleDescriptor *module)
{
    UmiModuleRuntimeEntry *entry;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || module == NULL ||
        module->module_id == NULL || module->module_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->started) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (controller->module_count >= UMI_MASTER_MAX_MODULES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (module->structure_size < sizeof(UmiModuleDescriptor) ||
        module->abi_version != UMICOM_FRAMEWORK_ABI_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_module_registry_add(controller->registry, module);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_master_register_module_capabilities(controller, module);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    entry = &controller->modules[controller->module_count++];
    (void)memset(entry, 0, sizeof(*entry));
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
    entry->context.capabilities = controller->capabilities;
    entry->context.services = controller->services;
    entry->context.command_registry = controller->command_registry;
    entry->context.health = controller->health;
    entry->context.policy = controller->policy;
    entry->context.diagnostic_sink = controller->diagnostic_sink;
    entry->context.diagnostic_user_data = controller->diagnostic_user_data;

    umi_master_health(controller,
                      module->module_id,
                      UMI_HEALTH_STOPPED,
                      "Module discovered");
    umi_master_report(controller,
                      UMI_DIAGNOSTIC_INFO,
                      "Slave Controller registered");
    return UMI_STATUS_OK;
}

/*
 * Provide the master module index operation used by this module and its client
 * applications.
 */
static int umi_master_module_index(
    const UmiMasterController *controller,
    const char *module_id,
    size_t *out_index)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < controller->module_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(controller->modules[index].descriptor->module_id,
                   module_id) == 0) {
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (out_index != NULL) {
                *out_index = index;
            }
            return 1;
        }
    }
    return 0;
}

/*
 * Provide the master dependencies ready operation used by this module and its client
 * applications.
 */
static int umi_master_dependencies_ready(
    const UmiMasterController *controller,
    const UmiModuleDescriptor *module,
    const int *scheduled)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (module->required_capabilities == NULL) {
        return 1;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; module->required_capabilities[index] != NULL; ++index) {
        const UmiCapabilityDescriptor *capability =
            umi_capability_registry_find(controller->capabilities,
                                         module->required_capabilities[index]);
        size_t provider_index;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (capability == NULL) {
            return 0;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((capability->flags & UMI_CAPABILITY_EXTERNAL) != 0U) {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_master_module_index(controller,
                                     capability->provider_module_id,
                                     &provider_index) ||
            !scheduled[provider_index]) {
            return 0;
        }
    }
    return 1;
}

/* Check that master controller satisfies its contract before another service relies on it. */
UmiStatus umi_master_controller_validate(
    UmiMasterController *controller,
    const char **out_missing_capability)
{
    int scheduled[UMI_MASTER_MAX_MODULES] = {0};
    size_t scheduled_count = 0U;
    size_t pass;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_missing_capability != NULL) {
        *out_missing_capability = NULL;
    }
    controller->start_order_count = 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < controller->module_count; ++index) {
        UmiStatus status = umi_capability_registry_require(
            controller->capabilities,
            controller->modules[index].descriptor->required_capabilities,
            out_missing_capability
        );
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (pass = 0U; pass < controller->module_count; ++pass) {
        int progress = 0;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < controller->module_count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!scheduled[index] &&
                umi_master_dependencies_ready(
                    controller,
                    controller->modules[index].descriptor,
                    scheduled)) {
                controller->start_order[controller->start_order_count++] = index;
                scheduled[index] = 1;
                scheduled_count += 1U;
                progress = 1;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (!progress) {
            break;
        }
    }

    return scheduled_count == controller->module_count
        ? UMI_STATUS_OK
        : UMI_STATUS_INVALID_STATE;
}

/* Provide the master run phase operation used by this module and its client applications. */
static UmiStatus umi_master_run_phase(
    UmiMasterController *controller,
    UmiModuleRuntimeEntry *entry,
    UmiModulePhaseFn phase,
    UmiModuleState success_state,
    UmiHealthState health_state,
    const char *success_message,
    const char *failure_message)
{
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (phase != NULL) {
        status = phase(&entry->context);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        entry->state = UMI_MODULE_FAILED;
        umi_master_health(controller,
                          entry->descriptor->module_id,
                          UMI_HEALTH_FAILED,
                          failure_message);
        umi_master_report(controller,
                          UMI_DIAGNOSTIC_ERROR,
                          failure_message);
        return status;
    }

    entry->state = success_state;
    umi_master_health(controller,
                      entry->descriptor->module_id,
                      health_state,
                      success_message);
    return UMI_STATUS_OK;
}

/*
 * Provide the master rollback start operation used by this module and its client
 * applications.
 */
static void umi_master_rollback_start(UmiMasterController *controller,
                                      size_t completed_entries)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = completed_entries; index > 0U; --index) {
        UmiModuleRuntimeEntry *entry =
            &controller->modules[controller->start_order[index - 1U]];

        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->state == UMI_MODULE_STARTED &&
            entry->descriptor->lifecycle.quiesce != NULL) {
            (void)entry->descriptor->lifecycle.quiesce(&entry->context);
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((entry->state == UMI_MODULE_STARTED ||
             entry->state == UMI_MODULE_QUIESCED ||
             entry->state == UMI_MODULE_INITIALISED ||
             entry->state == UMI_MODULE_CONFIGURED) &&
            entry->descriptor->lifecycle.stop != NULL) {
            (void)entry->descriptor->lifecycle.stop(&entry->context);
        }
        entry->state = UMI_MODULE_STOPPED;
        umi_master_health(controller,
                          entry->descriptor->module_id,
                          UMI_HEALTH_STOPPED,
                          "Module rolled back after startup failure");
    }
}

/*
 * Provide the master controller start operation used by this module and its client
 * applications.
 */
UmiStatus umi_master_controller_start(UmiMasterController *controller)
{
    size_t order_index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->started) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_master_controller_validate(controller, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_master_health(controller,
                          "org.umicom.framework.master",
                          UMI_HEALTH_FAILED,
                          "Module dependency validation failed");
        umi_master_report(controller,
                          UMI_DIAGNOSTIC_ERROR,
                          "Module dependency validation failed");
        return status;
    }

    umi_master_health(controller,
                      "org.umicom.framework.master",
                      UMI_HEALTH_STARTING,
                      "Starting Slave Controllers");

    /* Visit each bounded item once so every record receives the same rule. */
    for (order_index = 0U;
         order_index < controller->start_order_count;
         ++order_index) {
        UmiModuleRuntimeEntry *entry =
            &controller->modules[controller->start_order[order_index]];

        umi_master_health(controller,
                          entry->descriptor->module_id,
                          UMI_HEALTH_STARTING,
                          "Configuring module");
        status = umi_master_run_phase(controller,
                                      entry,
                                      entry->descriptor->lifecycle.configure,
                                      UMI_MODULE_CONFIGURED,
                                      UMI_HEALTH_STARTING,
                                      "Module configured",
                                      "Module configure failed");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            umi_master_rollback_start(controller, order_index);
            return status;
        }

        status = umi_master_run_phase(controller,
                                      entry,
                                      entry->descriptor->lifecycle.initialise,
                                      UMI_MODULE_INITIALISED,
                                      UMI_HEALTH_STARTING,
                                      "Module initialised",
                                      "Module initialise failed");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            umi_master_rollback_start(controller, order_index + 1U);
            return status;
        }

        status = umi_master_run_phase(controller,
                                      entry,
                                      entry->descriptor->lifecycle.start,
                                      UMI_MODULE_STARTED,
                                      UMI_HEALTH_READY,
                                      "Module started",
                                      "Module start failed");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            umi_master_rollback_start(controller, order_index + 1U);
            return status;
        }
    }

    controller->started = 1;
    umi_master_health(controller,
                      "org.umicom.framework.master",
                      UMI_HEALTH_READY,
                      "All Slave Controllers started");
    umi_master_report(controller,
                      UMI_DIAGNOSTIC_INFO,
                      "All Slave Controllers started");
    return UMI_STATUS_OK;
}

/*
 * Provide the master controller stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_master_controller_stop(UmiMasterController *controller)
{
    size_t order_index;
    UmiStatus first_failure = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!controller->started) {
        return UMI_STATUS_INVALID_STATE;
    }

    umi_master_health(controller,
                      "org.umicom.framework.master",
                      UMI_HEALTH_STOPPING,
                      "Stopping Slave Controllers");

    /* Visit each bounded item once so every record receives the same rule. */
    for (order_index = controller->start_order_count;
         order_index > 0U;
         --order_index) {
        UmiModuleRuntimeEntry *entry =
            &controller->modules[controller->start_order[order_index - 1U]];
        UmiStatus status;

        umi_master_health(controller,
                          entry->descriptor->module_id,
                          UMI_HEALTH_STOPPING,
                          "Stopping module");

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (entry->descriptor->lifecycle.quiesce != NULL) {
            status = entry->descriptor->lifecycle.quiesce(&entry->context);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) {
                entry->state = UMI_MODULE_QUIESCED;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                entry->state = UMI_MODULE_FAILED;
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (first_failure == UMI_STATUS_OK) {
                    first_failure = status;
                }
            }
        }

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (entry->descriptor->lifecycle.stop != NULL) {
            status = entry->descriptor->lifecycle.stop(&entry->context);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                entry->state = UMI_MODULE_FAILED;
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (first_failure == UMI_STATUS_OK) {
                    first_failure = status;
                }
                umi_master_health(controller,
                                  entry->descriptor->module_id,
                                  UMI_HEALTH_FAILED,
                                  "Module stop failed");
                continue;
            }
        }

        entry->state = UMI_MODULE_STOPPED;
        umi_master_health(controller,
                          entry->descriptor->module_id,
                          UMI_HEALTH_STOPPED,
                          "Module stopped");
    }

    controller->started = 0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (first_failure == UMI_STATUS_OK) {
        umi_master_health(controller,
                          "org.umicom.framework.master",
                          UMI_HEALTH_STOPPED,
                          "All Slave Controllers stopped");
        umi_master_report(controller,
                          UMI_DIAGNOSTIC_INFO,
                          "All Slave Controllers stopped");
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        umi_master_health(controller,
                          "org.umicom.framework.master",
                          UMI_HEALTH_DEGRADED,
                          "One or more Slave Controllers failed to stop cleanly");
    }
    return first_failure;
}

/*
 * Provide the master controller module state operation used by this module and its client
 * applications.
 */
UmiModuleState umi_master_controller_module_state(
    const UmiMasterController *controller,
    const char *module_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || module_id == NULL) {
        return UMI_MODULE_FAILED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < controller->module_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(controller->modules[index].descriptor->module_id,
                   module_id) == 0) {
            return controller->modules[index].state;
        }
    }
    return UMI_MODULE_FAILED;
}

/*
 * Return the number of records represented by master controller module without changing
 * their state.
 */
size_t umi_master_controller_module_count(
    const UmiMasterController *controller)
{
    return controller != NULL ? controller->module_count : 0U;
}

/*
 * Provide the master controller application name operation used by this module and its
 * client applications.
 */
const char *umi_master_controller_application_name(
    const UmiMasterController *controller)
{
    return controller != NULL ? controller->application_name : "";
}

/*
 * Provide the master controller events operation used by this module and its client
 * applications.
 */
UmiEventBus *umi_master_controller_events(UmiMasterController *controller)
{ return controller != NULL ? controller->events : NULL; }

/*
 * Provide the master controller commands operation used by this module and its client
 * applications.
 */
UmiCommandBus *umi_master_controller_commands(UmiMasterController *controller)
{ return controller != NULL ? controller->commands : NULL; }

/*
 * Provide the master controller queries operation used by this module and its client
 * applications.
 */
UmiQueryBus *umi_master_controller_queries(UmiMasterController *controller)
{ return controller != NULL ? controller->queries : NULL; }

/*
 * Provide the master controller data server operation used by this module and its client
 * applications.
 */
UmiDataServer *umi_master_controller_data_server(UmiMasterController *controller)
{ return controller != NULL ? controller->data_server : NULL; }

/*
 * Provide the master controller config operation used by this module and its client
 * applications.
 */
UmiConfig *umi_master_controller_config(UmiMasterController *controller)
{ return controller != NULL ? controller->config : NULL; }

/*
 * Provide the master controller clock operation used by this module and its client
 * applications.
 */
UmiClock *umi_master_controller_clock(UmiMasterController *controller)
{ return controller != NULL ? &controller->clock : NULL; }

/*
 * Provide the master controller scheduler operation used by this module and its client
 * applications.
 */
UmiScheduler *umi_master_controller_scheduler(UmiMasterController *controller)
{ return controller != NULL ? controller->scheduler : NULL; }

/*
 * Provide the master controller capabilities operation used by this module and its client
 * applications.
 */
UmiCapabilityRegistry *umi_master_controller_capabilities(
    UmiMasterController *controller)
{ return controller != NULL ? controller->capabilities : NULL; }

/*
 * Provide the master controller services operation used by this module and its client
 * applications.
 */
UmiServiceRegistry *umi_master_controller_services(
    UmiMasterController *controller)
{ return controller != NULL ? controller->services : NULL; }

/*
 * Provide the master controller command registry operation used by this module and its
 * client applications.
 */
UmiCommandRegistry *umi_master_controller_command_registry(
    UmiMasterController *controller)
{ return controller != NULL ? controller->command_registry : NULL; }

/*
 * Provide the master controller health operation used by this module and its client
 * applications.
 */
UmiHealthRegistry *umi_master_controller_health(
    UmiMasterController *controller)
{ return controller != NULL ? controller->health : NULL; }

/*
 * Provide the master controller policy operation used by this module and its client
 * applications.
 */
UmiPolicyEngine *umi_master_controller_policy(UmiMasterController *controller)
{ return controller != NULL ? controller->policy : NULL; }
