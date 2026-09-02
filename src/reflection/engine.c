/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/reflection/engine.c
 *
 * PURPOSE:
 *   Coordinate owned reflection metadata with the Framework change broker so
 *   module and resource discovery can update live application catalogues.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/reflection/engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiReflectionEngine {
    UmiReflectionRegistry *registry;
    UmiChangeBroker *change_broker;
    int require_change_record;
};

/* Copy pointer-based runtime text into a bounded reflection field. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    int written;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    if (written < 0 || (size_t)written >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Publish a small payload containing identifiers rather than reflected data. */
static UmiStatus publish_change(UmiReflectionEngine *engine,
                                const char *topic,
                                const char *item_id,
                                const char *module_id)
{
    char payload[512];
    int written;
    UmiStatus status;

    if (engine->change_broker == NULL) {
        return UMI_STATUS_OK;
    }
    written = snprintf(
        payload,
        sizeof(payload),
        "{\"id\":\"%s\",\"module_id\":\"%s\",\"registry_revision\":%llu}",
        item_id,
        module_id != NULL ? module_id : "",
        (unsigned long long)umi_reflection_registry_revision(engine->registry));
    if (written < 0 || (size_t)written >= sizeof(payload)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_change_broker_publish(
        engine->change_broker,
        topic,
        payload,
        umi_reflection_registry_revision(engine->registry));
    if (status != UMI_STATUS_OK && !engine->require_change_record) {
        return UMI_STATUS_OK;
    }
    return status;
}

UmiReflectionEngineConfig umi_reflection_engine_config_default(void)
{
    UmiReflectionEngineConfig config;

    (void)memset(&config, 0, sizeof(config));
    config.structure_size = (uint32_t)sizeof(config);
    config.registry = umi_reflection_registry_config_default();
    return config;
}

UmiStatus umi_reflection_engine_create(
    const UmiReflectionEngineConfig *config,
    UmiReflectionEngine **out_engine)
{
    UmiReflectionEngineConfig effective;
    UmiReflectionEngine *engine;
    UmiStatus status;

    if (out_engine == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_engine = NULL;
    if (config != NULL && config->structure_size < sizeof(*config)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = config != NULL
        ? *config
        : umi_reflection_engine_config_default();
    engine = (UmiReflectionEngine *)calloc(1U, sizeof(*engine));
    if (engine == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = umi_reflection_registry_create(
        &effective.registry, &engine->registry);
    if (status != UMI_STATUS_OK) {
        free(engine);
        return status;
    }
    engine->change_broker = effective.change_broker;
    engine->require_change_record = effective.require_change_record != 0;
    *out_engine = engine;
    return UMI_STATUS_OK;
}

void umi_reflection_engine_destroy(UmiReflectionEngine *engine)
{
    if (engine == NULL) {
        return;
    }
    umi_reflection_registry_destroy(engine->registry);
    free(engine);
}

UmiStatus umi_reflection_engine_register_type(
    UmiReflectionEngine *engine,
    const UmiReflectionTypeDescriptor *descriptor)
{
    UmiStatus status;

    if (engine == NULL || descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_reflection_registry_register_type(
        engine->registry, descriptor);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = publish_change(engine,
                            UMI_REFLECTION_TOPIC_TYPE_REGISTERED,
                            descriptor->type_id,
                            descriptor->module_id);
    if (status != UMI_STATUS_OK) {
        (void)umi_reflection_registry_unregister_type(
            engine->registry, descriptor->type_id);
    }
    return status;
}

UmiStatus umi_reflection_engine_unregister_type(
    UmiReflectionEngine *engine,
    const char *type_id)
{
    UmiReflectionTypeSnapshot previous;
    UmiStatus status;

    if (engine == NULL || type_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_reflection_registry_get_type(
        engine->registry, type_id, &previous);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_reflection_registry_unregister_type(engine->registry, type_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = publish_change(engine,
                            UMI_REFLECTION_TOPIC_TYPE_REMOVED,
                            type_id,
                            previous.descriptor.module_id);
    if (status != UMI_STATUS_OK) {
        (void)umi_reflection_registry_register_type(
            engine->registry, &previous.descriptor);
    }
    return status;
}

UmiStatus umi_reflection_engine_register_module(
    UmiReflectionEngine *engine,
    const UmiReflectionModuleDescriptor *descriptor)
{
    UmiStatus status;

    if (engine == NULL || descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_reflection_registry_register_module(
        engine->registry, descriptor);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = publish_change(engine,
                            UMI_REFLECTION_TOPIC_MODULE_REGISTERED,
                            descriptor->module_id,
                            descriptor->module_id);
    if (status != UMI_STATUS_OK) {
        (void)umi_reflection_registry_unregister_module(
            engine->registry, descriptor->module_id);
    }
    return status;
}

UmiStatus umi_reflection_engine_register_runtime_module(
    UmiReflectionEngine *engine,
    const UmiModuleDescriptor *descriptor,
    const char *description,
    uint32_t flags)
{
    UmiReflectionModuleDescriptor reflected;
    size_t index;
    UmiStatus status;

    if (engine == NULL || descriptor == NULL ||
        descriptor->structure_size < sizeof(*descriptor) ||
        descriptor->module_id == NULL || descriptor->display_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&reflected, 0, sizeof(reflected));
    reflected.structure_size = (uint32_t)sizeof(reflected);
    reflected.version = descriptor->module_version;
    reflected.abi_version = descriptor->abi_version;
    reflected.module_kind = (uint32_t)descriptor->kind;
    reflected.flags = flags;
    status = copy_text(reflected.module_id,
                       sizeof(reflected.module_id),
                       descriptor->module_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(reflected.display_name,
                           sizeof(reflected.display_name),
                           descriptor->display_name);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(reflected.description,
                           sizeof(reflected.description),
                           description != NULL ? description : "Runtime module");
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Provided capabilities form the public module surface visible to tools. */
    if (descriptor->provided_capabilities != NULL) {
        for (index = 0U;
             index < UMI_REFLECTION_MAX_CAPABILITIES &&
             descriptor->provided_capabilities[index] != NULL;
             ++index) {
            status = copy_text(reflected.capabilities[index],
                               sizeof(reflected.capabilities[index]),
                               descriptor->provided_capabilities[index]);
            if (status != UMI_STATUS_OK) {
                return status;
            }
            reflected.capability_count += 1U;
        }
    }
    return umi_reflection_engine_register_module(engine, &reflected);
}

UmiStatus umi_reflection_engine_unregister_module(
    UmiReflectionEngine *engine,
    const char *module_id)
{
    UmiReflectionModuleDescriptor previous;
    UmiStatus status;

    if (engine == NULL || module_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_reflection_registry_get_module(
        engine->registry, module_id, &previous);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_reflection_registry_unregister_module(
        engine->registry, module_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = publish_change(engine,
                            UMI_REFLECTION_TOPIC_MODULE_REMOVED,
                            module_id,
                            module_id);
    if (status != UMI_STATUS_OK) {
        (void)umi_reflection_registry_register_module(
            engine->registry, &previous);
    }
    return status;
}

UmiStatus umi_reflection_engine_register_resource(
    UmiReflectionEngine *engine,
    const UmiReflectionResourceDescriptor *descriptor)
{
    UmiStatus status;

    if (engine == NULL || descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_reflection_registry_register_resource(
        engine->registry, descriptor);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = publish_change(engine,
                            UMI_REFLECTION_TOPIC_RESOURCE_REGISTERED,
                            descriptor->resource_id,
                            descriptor->module_id);
    if (status != UMI_STATUS_OK) {
        (void)umi_reflection_registry_unregister_resource(
            engine->registry, descriptor->resource_id);
    }
    return status;
}

UmiStatus umi_reflection_engine_upsert_resource(
    UmiReflectionEngine *engine,
    const UmiReflectionResourceDescriptor *descriptor,
    int *out_replaced)
{
    UmiReflectionResourceDescriptor previous;
    UmiStatus previous_status;
    UmiStatus status;
    int replaced = 0;

    if (engine == NULL || descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    previous_status = umi_reflection_registry_get_resource(
        engine->registry, descriptor->resource_id, &previous);
    status = umi_reflection_registry_upsert_resource(
        engine->registry, descriptor, &replaced);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = publish_change(
        engine,
        replaced != 0
            ? UMI_REFLECTION_TOPIC_RESOURCE_CHANGED
            : UMI_REFLECTION_TOPIC_RESOURCE_REGISTERED,
        descriptor->resource_id,
        descriptor->module_id);
    if (status != UMI_STATUS_OK) {
        if (previous_status == UMI_STATUS_OK) {
            (void)umi_reflection_registry_upsert_resource(
                engine->registry, &previous, NULL);
        } else {
            (void)umi_reflection_registry_unregister_resource(
                engine->registry, descriptor->resource_id);
        }
        return status;
    }
    if (out_replaced != NULL) {
        *out_replaced = replaced;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_engine_unregister_resource(
    UmiReflectionEngine *engine,
    const char *resource_id)
{
    UmiReflectionResourceDescriptor previous;
    UmiStatus status;

    if (engine == NULL || resource_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_reflection_registry_get_resource(
        engine->registry, resource_id, &previous);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_reflection_registry_unregister_resource(
        engine->registry, resource_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = publish_change(engine,
                            UMI_REFLECTION_TOPIC_RESOURCE_REMOVED,
                            resource_id,
                            previous.module_id);
    if (status != UMI_STATUS_OK) {
        (void)umi_reflection_registry_register_resource(
            engine->registry, &previous);
    }
    return status;
}

UmiReflectionRegistry *umi_reflection_engine_registry(
    UmiReflectionEngine *engine)
{
    return engine != NULL ? engine->registry : NULL;
}

UmiChangeBroker *umi_reflection_engine_change_broker(
    UmiReflectionEngine *engine)
{
    return engine != NULL ? engine->change_broker : NULL;
}
