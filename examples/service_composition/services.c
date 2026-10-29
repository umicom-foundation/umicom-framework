/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/service_composition/services.c
 *
 * PURPOSE:
 *   Compose an owned counter service and a dependent module through Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "composition.h"
#include <stdint.h>
#include <stdlib.h>

#define PROVIDER_ID "example.counter.provider"
#define CONSUMER_ID "example.counter.consumer"
#define SERVICE_ID "example.counter"
static const char *const provided[] = {SERVICE_ID, NULL};
static const char *const required[] = {SERVICE_ID, NULL};
/* A duplicate within one module is rejected regardless of other providers.
 * The first entry must not leak into the registry when the second fails. */
static const char *const invalid_provided[] = {
    "example.consumer.marker", "example.consumer.marker", NULL
};

static void DestroyCounter(void *service, void *user_data)
{
    ExampleComposition *composition = user_data;
    ++composition->counters_destroyed;
    free(service);
}

static UmiStatus ConfigureProvider(UmiModuleContext *context)
{
    ExampleComposition *composition = context->module_state;
    ExampleCounter *counter = calloc(1U, sizeof(*counter));
    if (counter == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiServiceDescriptor service = {0};
    service.structure_size = (uint32_t)sizeof(service);
    service.service_id = SERVICE_ID;
    service.service_version = (UmiVersion){1U, 0U, 0U};
    service.provider_module_id = PROVIDER_ID;
    service.service = counter;
    service.destroy = DestroyCounter;
    service.destroy_user_data = composition;
    service.flags = UMI_SERVICE_OWNED | UMI_SERVICE_SINGLETON;
    UmiStatus status = umi_service_registry_register(context->services, &service);
    if (status != UMI_STATUS_OK) {
        /* A rejected registration never transfers ownership. Configure must
         * unwind its own partial allocation because configure did not succeed. */
        free(counter);
        return status;
    }
    ++composition->counters_created;
    composition->service_registered = 1;
    return UMI_STATUS_OK;
}

static UmiStatus QuiesceProvider(UmiModuleContext *context)
{
    ExampleComposition *composition = context->module_state;
    /* The provider cannot disappear while its consumer still borrows it.
     * Dependency-ordered shutdown stops the consumer before reaching here. */
    return composition->consumer_running
        ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}

static UmiStatus StopProvider(UmiModuleContext *context)
{
    ExampleComposition *composition = context->module_state;
    if (!composition->service_registered) return UMI_STATUS_OK;
    if (composition->borrowed_counter != NULL) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = umi_service_registry_unregister(
        context->services, SERVICE_ID, PROVIDER_ID);
    if (status == UMI_STATUS_OK) composition->service_registered = 0;
    return status;
}

static UmiStatus ConfigureConsumer(UmiModuleContext *context)
{
    ExampleComposition *composition = context->module_state;
    const UmiServiceDescriptor *service = umi_service_registry_find_provider(
        context->services, SERVICE_ID, PROVIDER_ID);
    if (service == NULL || service->service == NULL) return UMI_STATUS_NOT_FOUND;
    composition->borrowed_counter = service->service;
    return UMI_STATUS_OK;
}

static UmiStatus StartConsumer(UmiModuleContext *context)
{
    ExampleComposition *composition = context->module_state;
    if (composition->borrowed_counter == NULL) return UMI_STATUS_INVALID_STATE;
    if (composition->fail_start_once) {
        composition->fail_start_once = 0;
        return UMI_STATUS_IO_ERROR;
    }
    ++composition->borrowed_counter->value;
    composition->consumer_running = 1;
    return UMI_STATUS_OK;
}

static UmiStatus StopConsumer(UmiModuleContext *context)
{
    ExampleComposition *composition = context->module_state;
    if (composition->fail_stop_once) {
        composition->fail_stop_once = 0;
        /* Keep every borrowed dependency valid until a later successful stop. */
        return UMI_STATUS_IO_ERROR;
    }
    composition->consumer_running = 0;
    composition->borrowed_counter = NULL;
    return UMI_STATUS_OK;
}

static void DestroyModule(UmiModuleContext *context)
{
    ExampleComposition *composition = context->module_state;
    /* Module descriptors and state are owned by main. Count destruction;
     * do not free either borrowed object. Service release owns the counter. */
    ++composition->modules_destroyed;
}

static UmiModuleDescriptor Module(ExampleComposition *composition,
                                  const char *id)
{
    UmiModuleDescriptor module = {0};
    module.structure_size = (uint32_t)sizeof(module);
    module.abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    module.module_id = id;
    module.display_name = id;
    module.module_version = (UmiVersion){1U, 0U, 0U};
    module.kind = UMI_MODULE_SERVICE;
    module.module_state = composition;
    module.lifecycle.destroy = DestroyModule;
    return module;
}

UmiStatus ExampleCompositionCreate(ExampleComposition *composition,
                                   int check_corrected_registration)
{
    if (composition == NULL || composition->master != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiMasterControllerConfig config = {"Service composition example", NULL, NULL};
    UmiStatus status = umi_master_controller_create(&config, &composition->master);
    if (status != UMI_STATUS_OK) return status;
    composition->provider = Module(composition, PROVIDER_ID);
    composition->provider.provided_capabilities = provided;
    composition->provider.lifecycle.configure = ConfigureProvider;
    composition->provider.lifecycle.quiesce = QuiesceProvider;
    composition->provider.lifecycle.stop = StopProvider;
    composition->consumer = Module(composition, CONSUMER_ID);
    composition->consumer.required_capabilities = required;
    composition->consumer.lifecycle.configure = ConfigureConsumer;
    composition->consumer.lifecycle.start = StartConsumer;
    composition->consumer.lifecycle.stop = StopConsumer;

    /* Register the dependent first. The dependency graph, not registration
     * order, determines which module can start and which must stop first. */
    if (check_corrected_registration) {
        UmiCapabilityRegistry *capabilities =
            umi_master_controller_capabilities(composition->master);
        size_t before = umi_capability_registry_count(capabilities);
        composition->consumer.provided_capabilities = invalid_provided;
        status = umi_master_controller_register(composition->master,
                                                &composition->consumer);
        if (status != UMI_STATUS_ALREADY_EXISTS ||
            umi_master_controller_module_count(composition->master) != 0U ||
            umi_capability_registry_count(capabilities) != before ||
            umi_capability_registry_find(capabilities, "example.consumer.marker") != NULL)
            return UMI_STATUS_INTERNAL_ERROR;
        /* A failed registration borrowed nothing. Correct the descriptor only
         * before admission; successfully registered descriptors stay immutable. */
        composition->consumer.provided_capabilities = NULL;
    }
    status = umi_master_controller_register(composition->master, &composition->consumer);
    if (status != UMI_STATUS_OK) return status;
    return umi_master_controller_register(composition->master, &composition->provider);
}
