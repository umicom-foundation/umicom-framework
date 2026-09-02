/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/schema.c
 *
 * PURPOSE:
 *   Implement schema registration, duplicate protection, version compatibility and message validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Initialise schema registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_schema_registry_create(UmiSchemaRegistry **out_registry)
{
    UmiSchemaRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiSchemaRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_spin_init(&registry->lock);
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/* Release or reset state held by schema registry so the same storage can be reused safely. */
void umi_schema_registry_destroy(UmiSchemaRegistry *registry)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        free(registry->entries[index].schema_id);
    }
    free(registry);
}

/* Add schema registry only after its inputs and available capacity have been checked. */
UmiStatus umi_schema_registry_register(UmiSchemaRegistry *registry,
                                       const UmiSchemaDescriptor *schema)
{
    size_t index;
    UmiOwnedSchema *entry;
    char *id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || schema == NULL || schema->schema_id == NULL ||
        schema->schema_id[0] == '\0' || schema->version == 0U ||
        schema->maximum_payload_size == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    id = umi_message_strdup(schema->schema_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_spin_lock(&registry->lock);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->entries[index].descriptor.version == schema->version &&
            strcmp(registry->entries[index].schema_id, schema->schema_id) == 0) {
            umi_spin_unlock(&registry->lock);
            free(id);
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/* Provide the compatible operation used by this module and its client applications. */
static int compatible(const UmiSchemaDescriptor *schema, uint32_t version)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (schema->version == version) return 1;
    /* Select the behaviour associated with the requested command or state value. */
    switch (schema->compatibility) {
    case UMI_SCHEMA_BACKWARD: return version <= schema->version;
    case UMI_SCHEMA_FORWARD: return version >= schema->version;
    case UMI_SCHEMA_FULL: return 1;
    default: return 0;
    }
}

/*
 * Find schema registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiSchemaDescriptor *umi_schema_registry_find(
    const UmiSchemaRegistry *registry,
    const char *schema_id,
    uint32_t version)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || schema_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        const UmiSchemaDescriptor *schema =
            &registry->entries[index].descriptor;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(schema->schema_id, schema_id) == 0 &&
            compatible(schema, version)) {
            return schema;
        }
    }
    return NULL;
}

/* Check that schema registry satisfies its contract before another service relies on it. */
UmiStatus umi_schema_registry_validate(const UmiSchemaRegistry *registry,
                                       const UmiMessageEnvelope *message)
{
    const UmiSchemaDescriptor *schema;
    UmiStatus status = umi_message_validate(message);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || message->schema_id == NULL ||
        message->schema_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    schema = umi_schema_registry_find(registry,
                                      message->schema_id,
                                      message->schema_version);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (schema == NULL) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (schema->kind != message->kind) return UMI_STATUS_INVALID_ARGUMENT;
    return message->payload_size <= schema->maximum_payload_size
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Return the number of records represented by schema registry without changing their
 * state.
 */
size_t umi_schema_registry_count(const UmiSchemaRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
