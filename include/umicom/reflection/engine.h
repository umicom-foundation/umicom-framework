/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/reflection/engine.h
 *
 * PURPOSE:
 *   Coordinate reflection registration, runtime-module projection and durable
 *   metadata change notifications through one reusable Framework service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REFLECTION_ENGINE_H
#define UMICOM_REFLECTION_ENGINE_H

#include "umicom/messaging/change_broker.h"
#include "umicom/reflection/registry.h"
#include "umicom/runtime/module.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REFLECTION_TOPIC_TYPE_REGISTERED \
    "umicom.reflection.type.registered"
#define UMI_REFLECTION_TOPIC_TYPE_REMOVED \
    "umicom.reflection.type.removed"
#define UMI_REFLECTION_TOPIC_MODULE_REGISTERED \
    "umicom.reflection.module.registered"
#define UMI_REFLECTION_TOPIC_MODULE_REMOVED \
    "umicom.reflection.module.removed"
#define UMI_REFLECTION_TOPIC_RESOURCE_REGISTERED \
    "umicom.reflection.resource.registered"
#define UMI_REFLECTION_TOPIC_RESOURCE_CHANGED \
    "umicom.reflection.resource.changed"
#define UMI_REFLECTION_TOPIC_RESOURCE_REMOVED \
    "umicom.reflection.resource.removed"

/** Keep orchestration state private while exposing the registry through queries. */
typedef struct UmiReflectionEngine UmiReflectionEngine;

/** Configure registry limits and optional borrowed change publication. */
typedef struct UmiReflectionEngineConfig {
    uint32_t structure_size;
    UmiReflectionRegistryConfig registry;
    UmiChangeBroker *change_broker;
    int require_change_record;
} UmiReflectionEngineConfig;

/** Return an embedded-engine configuration with conservative capacity limits. */
UmiReflectionEngineConfig umi_reflection_engine_config_default(void);

/** Create an empty reflection engine; the optional broker remains caller-owned. */
UmiStatus umi_reflection_engine_create(
    const UmiReflectionEngineConfig *config,
    UmiReflectionEngine **out_engine);

/** Release the engine registry without destroying its borrowed message broker. */
void umi_reflection_engine_destroy(UmiReflectionEngine *engine);

/** Register one type and publish its availability after the registry accepts it. */
UmiStatus umi_reflection_engine_register_type(
    UmiReflectionEngine *engine,
    const UmiReflectionTypeDescriptor *descriptor);

/** Remove one type and publish the change to interested application surfaces. */
UmiStatus umi_reflection_engine_unregister_type(
    UmiReflectionEngine *engine,
    const char *type_id);

/** Register already-owned module metadata. */
UmiStatus umi_reflection_engine_register_module(
    UmiReflectionEngine *engine,
    const UmiReflectionModuleDescriptor *descriptor);

/**
 * Copy the safe descriptive portion of a runtime module. Lifecycle functions
 * and module-state pointers deliberately remain outside reflection storage.
 */
UmiStatus umi_reflection_engine_register_runtime_module(
    UmiReflectionEngine *engine,
    const UmiModuleDescriptor *descriptor,
    const char *description,
    uint32_t flags);

/** Remove reflected module metadata without unloading executable code. */
UmiStatus umi_reflection_engine_unregister_module(
    UmiReflectionEngine *engine,
    const char *module_id);

/** Register one new layout, panel, service, schema, theme or document resource. */
UmiStatus umi_reflection_engine_register_resource(
    UmiReflectionEngine *engine,
    const UmiReflectionResourceDescriptor *descriptor);

/** Insert or refresh resource metadata and publish the appropriate change topic. */
UmiStatus umi_reflection_engine_upsert_resource(
    UmiReflectionEngine *engine,
    const UmiReflectionResourceDescriptor *descriptor,
    int *out_replaced);

/** Remove a resource and tell subscribers that it is no longer discoverable. */
UmiStatus umi_reflection_engine_unregister_resource(
    UmiReflectionEngine *engine,
    const char *resource_id);

/** Return the engine-owned registry for read queries and caller-owned snapshots. */
UmiReflectionRegistry *umi_reflection_engine_registry(
    UmiReflectionEngine *engine);

/** Return the optional borrowed broker used for change subscriptions. */
UmiChangeBroker *umi_reflection_engine_change_broker(
    UmiReflectionEngine *engine);

#ifdef __cplusplus
}
#endif

#endif
