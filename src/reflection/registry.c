/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/reflection/registry.c
 *
 * PURPOSE:
 *   Own and synchronise reflection metadata so callers never depend on a
 *   plug-in's temporary arrays or on pointers invalidated by module unloading.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/reflection/registry.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

typedef struct UmiOwnedReflectionType {
    UmiReflectionTypeDescriptor descriptor;
    UmiReflectionFieldDescriptor *fields;
    UmiReflectionAttribute *attributes;
} UmiOwnedReflectionType;

struct UmiReflectionRegistry {
    UmiOwnedReflectionType *types;
    UmiReflectionModuleDescriptor *modules;
    UmiReflectionResourceDescriptor *resources;
    size_t type_capacity;
    size_t module_capacity;
    size_t resource_capacity;
    size_t type_count;
    size_t module_count;
    size_t resource_count;
    uint64_t revision;
    UmiMutex *mutex;
};

/* Compare ASCII catalogue text without making queries depend on the locale. */
static int text_contains_case_insensitive(const char *text, const char *needle)
{
    size_t text_index;
    size_t needle_length;

    if (needle == NULL || needle[0] == '\0') {
        return 1;
    }
    if (text == NULL) {
        return 0;
    }
    needle_length = strlen(needle);
    for (text_index = 0U; text[text_index] != '\0'; ++text_index) {
        size_t needle_index = 0U;
        while (needle_index < needle_length &&
               text[text_index + needle_index] != '\0' &&
               tolower((unsigned char)text[text_index + needle_index]) ==
                   tolower((unsigned char)needle[needle_index])) {
            needle_index += 1U;
        }
        if (needle_index == needle_length) {
            return 1;
        }
    }
    return 0;
}

/* Locate a type while the caller holds the registry mutex. */
static size_t find_type_locked(const UmiReflectionRegistry *registry,
                               const char *type_id)
{
    size_t index;

    for (index = 0U; index < registry->type_count; ++index) {
        if (strcmp(registry->types[index].descriptor.type_id, type_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Locate a module while the caller holds the registry mutex. */
static size_t find_module_locked(const UmiReflectionRegistry *registry,
                                 const char *module_id)
{
    size_t index;

    for (index = 0U; index < registry->module_count; ++index) {
        if (strcmp(registry->modules[index].module_id, module_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Locate a resource while the caller holds the registry mutex. */
static size_t find_resource_locked(const UmiReflectionRegistry *registry,
                                   const char *resource_id)
{
    size_t index;

    for (index = 0U; index < registry->resource_count; ++index) {
        if (strcmp(registry->resources[index].resource_id, resource_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Release the variable-length arrays owned by one reflected type. */
static void owned_type_dispose(UmiOwnedReflectionType *type)
{
    if (type == NULL) {
        return;
    }
    free(type->fields);
    free(type->attributes);
    (void)memset(type, 0, sizeof(*type));
}

/* Deep-copy a validated type before acquiring the registry lock. */
static UmiStatus owned_type_copy(const UmiReflectionTypeDescriptor *source,
                                 UmiOwnedReflectionType *destination)
{
    (void)memset(destination, 0, sizeof(*destination));
    destination->descriptor = *source;
    if (source->field_count > 0U) {
        destination->fields = (UmiReflectionFieldDescriptor *)malloc(
            source->field_count * sizeof(*destination->fields));
        if (destination->fields == NULL) {
            owned_type_dispose(destination);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        (void)memcpy(destination->fields,
                     source->fields,
                     source->field_count * sizeof(*destination->fields));
        destination->descriptor.fields = destination->fields;
    }
    if (source->attribute_count > 0U) {
        destination->attributes = (UmiReflectionAttribute *)malloc(
            source->attribute_count * sizeof(*destination->attributes));
        if (destination->attributes == NULL) {
            owned_type_dispose(destination);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        (void)memcpy(destination->attributes,
                     source->attributes,
                     source->attribute_count * sizeof(*destination->attributes));
        destination->descriptor.attributes = destination->attributes;
    }
    return UMI_STATUS_OK;
}

UmiReflectionRegistryConfig umi_reflection_registry_config_default(void)
{
    UmiReflectionRegistryConfig config;

    config.structure_size = (uint32_t)sizeof(config);
    config.type_capacity = 512U;
    config.module_capacity = 128U;
    config.resource_capacity = 2048U;
    return config;
}

UmiStatus umi_reflection_registry_create(
    const UmiReflectionRegistryConfig *config,
    UmiReflectionRegistry **out_registry)
{
    UmiReflectionRegistryConfig effective;
    UmiReflectionRegistry *registry;
    UmiStatus status;

    if (out_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = NULL;
    if (config != NULL && config->structure_size < sizeof(*config)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = config != NULL
        ? *config
        : umi_reflection_registry_config_default();
    if (effective.type_capacity == 0U || effective.module_capacity == 0U ||
        effective.resource_capacity == 0U ||
        effective.type_capacity > SIZE_MAX / sizeof(UmiOwnedReflectionType) ||
        effective.module_capacity >
            SIZE_MAX / sizeof(UmiReflectionModuleDescriptor) ||
        effective.resource_capacity >
            SIZE_MAX / sizeof(UmiReflectionResourceDescriptor)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = (UmiReflectionRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    registry->types = (UmiOwnedReflectionType *)calloc(
        effective.type_capacity, sizeof(*registry->types));
    registry->modules = (UmiReflectionModuleDescriptor *)calloc(
        effective.module_capacity, sizeof(*registry->modules));
    registry->resources = (UmiReflectionResourceDescriptor *)calloc(
        effective.resource_capacity, sizeof(*registry->resources));
    if (registry->types == NULL || registry->modules == NULL ||
        registry->resources == NULL) {
        umi_reflection_registry_destroy(registry);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    registry->type_capacity = effective.type_capacity;
    registry->module_capacity = effective.module_capacity;
    registry->resource_capacity = effective.resource_capacity;
    registry->revision = 1U;
    status = umi_mutex_create(&registry->mutex);
    if (status != UMI_STATUS_OK) {
        umi_reflection_registry_destroy(registry);
        return status;
    }
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_reflection_registry_destroy(UmiReflectionRegistry *registry)
{
    size_t index;

    if (registry == NULL) {
        return;
    }
    for (index = 0U; index < registry->type_count; ++index) {
        owned_type_dispose(&registry->types[index]);
    }
    umi_mutex_destroy(registry->mutex);
    free(registry->types);
    free(registry->modules);
    free(registry->resources);
    free(registry);
}

UmiStatus umi_reflection_registry_register_type(
    UmiReflectionRegistry *registry,
    const UmiReflectionTypeDescriptor *descriptor)
{
    UmiOwnedReflectionType copy;
    UmiStatus status;

    if (registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_reflection_type_descriptor_validate(descriptor);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = owned_type_copy(descriptor, &copy);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        owned_type_dispose(&copy);
        return status;
    }
    if (find_type_locked(registry, descriptor->type_id) != SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        owned_type_dispose(&copy);
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->type_count >= registry->type_capacity) {
        (void)umi_mutex_unlock(registry->mutex);
        owned_type_dispose(&copy);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->types[registry->type_count] = copy;
    registry->type_count += 1U;
    registry->revision += 1U;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_registry_unregister_type(
    UmiReflectionRegistry *registry,
    const char *type_id)
{
    size_t index;
    size_t move_index;
    UmiStatus status;

    if (registry == NULL || type_id == NULL || type_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    index = find_type_locked(registry, type_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    owned_type_dispose(&registry->types[index]);
    for (move_index = index + 1U;
         move_index < registry->type_count;
         ++move_index) {
        registry->types[move_index - 1U] = registry->types[move_index];
    }
    registry->type_count -= 1U;
    (void)memset(&registry->types[registry->type_count],
                 0,
                 sizeof(registry->types[registry->type_count]));
    registry->revision += 1U;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_registry_get_type(
    const UmiReflectionRegistry *registry,
    const char *type_id,
    UmiReflectionTypeSnapshot *out_snapshot)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL || type_id == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    index = find_type_locked(registry, type_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->descriptor = registry->types[index].descriptor;
    if (out_snapshot->descriptor.field_count > 0U) {
        (void)memcpy(out_snapshot->fields,
                     registry->types[index].fields,
                     out_snapshot->descriptor.field_count *
                         sizeof(*out_snapshot->fields));
        out_snapshot->descriptor.fields = out_snapshot->fields;
    }
    if (out_snapshot->descriptor.attribute_count > 0U) {
        (void)memcpy(out_snapshot->attributes,
                     registry->types[index].attributes,
                     out_snapshot->descriptor.attribute_count *
                         sizeof(*out_snapshot->attributes));
        out_snapshot->descriptor.attributes = out_snapshot->attributes;
    }
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_registry_register_module(
    UmiReflectionRegistry *registry,
    const UmiReflectionModuleDescriptor *descriptor)
{
    UmiStatus status;

    if (registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_reflection_module_descriptor_validate(descriptor);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (find_module_locked(registry, descriptor->module_id) != SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->module_count >= registry->module_capacity) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->modules[registry->module_count] = *descriptor;
    registry->modules[registry->module_count].structure_size =
        (uint32_t)sizeof(*descriptor);
    registry->module_count += 1U;
    registry->revision += 1U;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_registry_unregister_module(
    UmiReflectionRegistry *registry,
    const char *module_id)
{
    size_t index;
    size_t move_index;
    UmiStatus status;

    if (registry == NULL || module_id == NULL || module_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    index = find_module_locked(registry, module_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    for (move_index = index + 1U;
         move_index < registry->module_count;
         ++move_index) {
        registry->modules[move_index - 1U] = registry->modules[move_index];
    }
    registry->module_count -= 1U;
    (void)memset(&registry->modules[registry->module_count],
                 0,
                 sizeof(registry->modules[registry->module_count]));
    registry->revision += 1U;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_registry_get_module(
    const UmiReflectionRegistry *registry,
    const char *module_id,
    UmiReflectionModuleDescriptor *out_descriptor)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL || module_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    index = find_module_locked(registry, module_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_descriptor = registry->modules[index];
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_registry_register_resource(
    UmiReflectionRegistry *registry,
    const UmiReflectionResourceDescriptor *descriptor)
{
    UmiStatus status;

    if (registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_reflection_resource_descriptor_validate(descriptor);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (find_resource_locked(registry, descriptor->resource_id) != SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->resource_count >= registry->resource_capacity) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->resources[registry->resource_count] = *descriptor;
    registry->resources[registry->resource_count].structure_size =
        (uint32_t)sizeof(*descriptor);
    registry->resource_count += 1U;
    registry->revision += 1U;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_registry_upsert_resource(
    UmiReflectionRegistry *registry,
    const UmiReflectionResourceDescriptor *descriptor,
    int *out_replaced)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_reflection_resource_descriptor_validate(descriptor);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    index = find_resource_locked(registry, descriptor->resource_id);
    if (index == SIZE_MAX) {
        if (registry->resource_count >= registry->resource_capacity) {
            (void)umi_mutex_unlock(registry->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->resource_count;
        registry->resource_count += 1U;
        if (out_replaced != NULL) {
            *out_replaced = 0;
        }
    } else if (out_replaced != NULL) {
        *out_replaced = 1;
    }
    registry->resources[index] = *descriptor;
    registry->resources[index].structure_size = (uint32_t)sizeof(*descriptor);
    registry->revision += 1U;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_registry_unregister_resource(
    UmiReflectionRegistry *registry,
    const char *resource_id)
{
    size_t index;
    size_t move_index;
    UmiStatus status;

    if (registry == NULL || resource_id == NULL || resource_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    index = find_resource_locked(registry, resource_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    for (move_index = index + 1U;
         move_index < registry->resource_count;
         ++move_index) {
        registry->resources[move_index - 1U] = registry->resources[move_index];
    }
    registry->resource_count -= 1U;
    (void)memset(&registry->resources[registry->resource_count],
                 0,
                 sizeof(registry->resources[registry->resource_count]));
    registry->revision += 1U;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_registry_get_resource(
    const UmiReflectionRegistry *registry,
    const char *resource_id,
    UmiReflectionResourceDescriptor *out_descriptor)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL || resource_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    index = find_resource_locked(registry, resource_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_descriptor = registry->resources[index];
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

/* Validate common query storage before a catalogue reads its fixed strings. */
static UmiStatus query_validate(const UmiReflectionQuery *query)
{
    if (query == NULL || query->structure_size < sizeof(*query) ||
        memchr(query->module_id, '\0', sizeof(query->module_id)) == NULL ||
        memchr(query->text, '\0', sizeof(query->text)) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Add one matching identifier after applying offset and result limits. */
static void query_add_identifier(const char *identifier,
                                 size_t match_index,
                                 const UmiReflectionQuery *query,
                                 UmiReflectionQueryResult *result)
{
    size_t limit = query->limit;

    if (limit == 0U || limit > UMI_REFLECTION_MAX_QUERY_RESULTS) {
        limit = UMI_REFLECTION_MAX_QUERY_RESULTS;
    }
    result->total_available += 1U;
    if (match_index < query->offset || result->count >= limit) {
        return;
    }
    (void)snprintf(result->ids[result->count],
                   sizeof(result->ids[result->count]),
                   "%s",
                   identifier);
    result->count += 1U;
}

UmiStatus umi_reflection_registry_query_types(
    const UmiReflectionRegistry *registry,
    const UmiReflectionQuery *query,
    UmiReflectionQueryResult *out_result)
{
    size_t index;
    size_t match_index = 0U;
    UmiStatus status;

    if (registry == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = query_validate(query);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->structure_size = (uint32_t)sizeof(*out_result);
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    for (index = 0U; index < registry->type_count; ++index) {
        const UmiReflectionTypeDescriptor *type =
            &registry->types[index].descriptor;
        if (query->module_id[0] != '\0' &&
            strcmp(query->module_id, type->module_id) != 0) {
            continue;
        }
        if (query->type_kind != 0 && query->type_kind != type->kind) {
            continue;
        }
        if (!text_contains_case_insensitive(type->type_id, query->text) &&
            !text_contains_case_insensitive(type->display_name, query->text) &&
            !text_contains_case_insensitive(type->description, query->text)) {
            continue;
        }
        query_add_identifier(type->type_id, match_index, query, out_result);
        match_index += 1U;
    }
    out_result->truncated =
        out_result->count < out_result->total_available -
            (query->offset < out_result->total_available
                 ? query->offset
                 : out_result->total_available);
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_reflection_registry_query_resources(
    const UmiReflectionRegistry *registry,
    const UmiReflectionQuery *query,
    UmiReflectionQueryResult *out_result)
{
    size_t index;
    size_t match_index = 0U;
    UmiStatus status;

    if (registry == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = query_validate(query);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->structure_size = (uint32_t)sizeof(*out_result);
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    for (index = 0U; index < registry->resource_count; ++index) {
        const UmiReflectionResourceDescriptor *resource =
            &registry->resources[index];
        if (query->module_id[0] != '\0' &&
            strcmp(query->module_id, resource->module_id) != 0) {
            continue;
        }
        if (query->resource_kind != 0 &&
            query->resource_kind != resource->kind) {
            continue;
        }
        if (!text_contains_case_insensitive(resource->resource_id, query->text) &&
            !text_contains_case_insensitive(resource->display_name, query->text) &&
            !text_contains_case_insensitive(resource->description, query->text)) {
            continue;
        }
        query_add_identifier(
            resource->resource_id, match_index, query, out_result);
        match_index += 1U;
    }
    out_result->truncated =
        out_result->count < out_result->total_available -
            (query->offset < out_result->total_available
                 ? query->offset
                 : out_result->total_available);
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

size_t umi_reflection_registry_type_count(
    const UmiReflectionRegistry *registry)
{
    size_t count = 0U;

    if (registry != NULL && umi_mutex_lock(registry->mutex) == UMI_STATUS_OK) {
        count = registry->type_count;
        (void)umi_mutex_unlock(registry->mutex);
    }
    return count;
}

size_t umi_reflection_registry_module_count(
    const UmiReflectionRegistry *registry)
{
    size_t count = 0U;

    if (registry != NULL && umi_mutex_lock(registry->mutex) == UMI_STATUS_OK) {
        count = registry->module_count;
        (void)umi_mutex_unlock(registry->mutex);
    }
    return count;
}

size_t umi_reflection_registry_resource_count(
    const UmiReflectionRegistry *registry)
{
    size_t count = 0U;

    if (registry != NULL && umi_mutex_lock(registry->mutex) == UMI_STATUS_OK) {
        count = registry->resource_count;
        (void)umi_mutex_unlock(registry->mutex);
    }
    return count;
}

uint64_t umi_reflection_registry_revision(
    const UmiReflectionRegistry *registry)
{
    uint64_t revision = 0U;

    if (registry != NULL && umi_mutex_lock(registry->mutex) == UMI_STATUS_OK) {
        revision = registry->revision;
        (void)umi_mutex_unlock(registry->mutex);
    }
    return revision;
}
