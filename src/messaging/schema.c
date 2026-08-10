/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/schema.c
 *
 * PURPOSE:
 *   Implement schema registration, duplicate protection, version compatibility and message validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/schema.h"

#include <stdlib.h>
#include <string.h>

#include "include/messaging_internal.h"

#define UMI_SCHEMA_CAPACITY 256U

typedef struct UmiOwnedSchema {
    UmiSchemaDescriptor descriptor;
    char *schema_id;
} UmiOwnedSchema;

struct UmiSchemaRegistry {
    UmiSpinLock lock;
    UmiOwnedSchema entries[UMI_SCHEMA_CAPACITY];
    size_t count;
};

UmiStatus umi_schema_registry_create(UmiSchemaRegistry **out_registry)
{
    UmiSchemaRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiSchemaRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_spin_init(&registry->lock);
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_schema_registry_destroy(UmiSchemaRegistry *registry)
{
    size_t index;
    if (registry == NULL) return;
    for (index = 0U; index < registry->count; ++index) {
        free(registry->entries[index].schema_id);
    }
    free(registry);
}

UmiStatus umi_schema_registry_register(UmiSchemaRegistry *registry,
                                       const UmiSchemaDescriptor *schema)
{
    size_t index;
    UmiOwnedSchema *entry;
    char *id;
    if (registry == NULL || schema == NULL || schema->schema_id == NULL ||
        schema->schema_id[0] == '\0' || schema->version == 0U ||
        schema->maximum_payload_size == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    id = umi_message_strdup(schema->schema_id);
    if (id == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_spin_lock(&registry->lock);
    for (index = 0U; index < registry->count; ++index) {
        if (registry->entries[index].descriptor.version == schema->version &&
            strcmp(registry->entries[index].schema_id, schema->schema_id) == 0) {
            umi_spin_unlock(&registry->lock);
            free(id);
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (registry->count >= UMI_SCHEMA_CAPACITY) {
        umi_spin_unlock(&registry->lock);
        free(id);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry = &registry->entries[registry->count++];
    entry->descriptor = *schema;
    entry->descriptor.structure_size = (uint32_t)sizeof(entry->descriptor);
    entry->schema_id = id;
    entry->descriptor.schema_id = id;
    umi_spin_unlock(&registry->lock);
    return UMI_STATUS_OK;
}

static int compatible(const UmiSchemaDescriptor *schema, uint32_t version)
{
    if (schema->version == version) return 1;
    switch (schema->compatibility) {
    case UMI_SCHEMA_BACKWARD: return version <= schema->version;
    case UMI_SCHEMA_FORWARD: return version >= schema->version;
    case UMI_SCHEMA_FULL: return 1;
    default: return 0;
    }
}

const UmiSchemaDescriptor *umi_schema_registry_find(
    const UmiSchemaRegistry *registry,
    const char *schema_id,
    uint32_t version)
{
    size_t index;
    if (registry == NULL || schema_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        const UmiSchemaDescriptor *schema =
            &registry->entries[index].descriptor;
        if (strcmp(schema->schema_id, schema_id) == 0 &&
            compatible(schema, version)) {
            return schema;
        }
    }
    return NULL;
}

UmiStatus umi_schema_registry_validate(const UmiSchemaRegistry *registry,
                                       const UmiMessageEnvelope *message)
{
    const UmiSchemaDescriptor *schema;
    UmiStatus status = umi_message_validate(message);
    if (status != UMI_STATUS_OK) return status;
    if (registry == NULL || message->schema_id == NULL ||
        message->schema_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    schema = umi_schema_registry_find(registry,
                                      message->schema_id,
                                      message->schema_version);
    if (schema == NULL) return UMI_STATUS_NOT_FOUND;
    if (schema->kind != message->kind) return UMI_STATUS_INVALID_ARGUMENT;
    return message->payload_size <= schema->maximum_payload_size
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

size_t umi_schema_registry_count(const UmiSchemaRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
