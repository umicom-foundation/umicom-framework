/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/reflection/registry.h
 *
 * PURPOSE:
 *   Provide an owned, searchable registry for reflected types, modules and
 *   data-driven resources shared by every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REFLECTION_REGISTRY_H
#define UMICOM_REFLECTION_REGISTRY_H

#include "umicom/reflection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Keep registry storage and locking private to the Framework. */
typedef struct UmiReflectionRegistry UmiReflectionRegistry;

/** Define explicit limits so discovery cannot consume unbounded memory. */
typedef struct UmiReflectionRegistryConfig {
    uint32_t structure_size;
    size_t type_capacity;
    size_t module_capacity;
    size_t resource_capacity;
} UmiReflectionRegistryConfig;

/** Return limits suitable for a complete local Umicom application suite. */
UmiReflectionRegistryConfig umi_reflection_registry_config_default(void);

/** Allocate an empty registry that owns every metadata copy it accepts. */
UmiStatus umi_reflection_registry_create(
    const UmiReflectionRegistryConfig *config,
    UmiReflectionRegistry **out_registry);

/** Release all owned field arrays, records and synchronisation state. */
void umi_reflection_registry_destroy(UmiReflectionRegistry *registry);

/** Register a validated type and deep-copy its fields and attributes. */
UmiStatus umi_reflection_registry_register_type(
    UmiReflectionRegistry *registry,
    const UmiReflectionTypeDescriptor *descriptor);

/** Remove one type while leaving all other registered metadata intact. */
UmiStatus umi_reflection_registry_unregister_type(
    UmiReflectionRegistry *registry,
    const char *type_id);

/** Copy one type and its arrays into caller-owned snapshot storage. */
UmiStatus umi_reflection_registry_get_type(
    const UmiReflectionRegistry *registry,
    const char *type_id,
    UmiReflectionTypeSnapshot *out_snapshot);

/** Register an inspectable module without retaining its executable pointers. */
UmiStatus umi_reflection_registry_register_module(
    UmiReflectionRegistry *registry,
    const UmiReflectionModuleDescriptor *descriptor);

/** Remove one module metadata record; executable unloading remains a plug-in job. */
UmiStatus umi_reflection_registry_unregister_module(
    UmiReflectionRegistry *registry,
    const char *module_id);

/** Copy one module descriptor into caller-owned storage. */
UmiStatus umi_reflection_registry_get_module(
    const UmiReflectionRegistry *registry,
    const char *module_id,
    UmiReflectionModuleDescriptor *out_descriptor);

/** Add a new data-driven resource such as a layout, panel or theme. */
UmiStatus umi_reflection_registry_register_resource(
    UmiReflectionRegistry *registry,
    const UmiReflectionResourceDescriptor *descriptor);

/** Insert a resource or replace its metadata when a watched file changes. */
UmiStatus umi_reflection_registry_upsert_resource(
    UmiReflectionRegistry *registry,
    const UmiReflectionResourceDescriptor *descriptor,
    int *out_replaced);

/** Remove a resource that disappeared from its discovery directory. */
UmiStatus umi_reflection_registry_unregister_resource(
    UmiReflectionRegistry *registry,
    const char *resource_id);

/** Copy one resource descriptor into caller-owned storage. */
UmiStatus umi_reflection_registry_get_resource(
    const UmiReflectionRegistry *registry,
    const char *resource_id,
    UmiReflectionResourceDescriptor *out_descriptor);

/** Find type identifiers using module, text, kind and paging filters. */
UmiStatus umi_reflection_registry_query_types(
    const UmiReflectionRegistry *registry,
    const UmiReflectionQuery *query,
    UmiReflectionQueryResult *out_result);

/** Find resource identifiers using module, text, kind and paging filters. */
UmiStatus umi_reflection_registry_query_resources(
    const UmiReflectionRegistry *registry,
    const UmiReflectionQuery *query,
    UmiReflectionQueryResult *out_result);

/** Return the number of types currently available to clients. */
size_t umi_reflection_registry_type_count(
    const UmiReflectionRegistry *registry);

/** Return the number of modules currently available to clients. */
size_t umi_reflection_registry_module_count(
    const UmiReflectionRegistry *registry);

/** Return the number of data-driven resources currently available to clients. */
size_t umi_reflection_registry_resource_count(
    const UmiReflectionRegistry *registry);

/** Return a monotonic revision changed by every successful mutation. */
uint64_t umi_reflection_registry_revision(
    const UmiReflectionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
