/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/connector.c
 *
 * PURPOSE:
 *   Implement copied connector discovery and safe open, execute and close
 *   dispatch while keeping secret material outside Framework data structures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/connector.h"

#include <stdlib.h>
#include <string.h>

/* Registry storage has fixed capacity so descriptor addresses never need grow. */
struct UmiDataConnectorRegistry {
    UmiDataConnectorDescriptor *descriptors;
    size_t capacity;
    size_t count;
};

/* Validate an adapter before copying its callback boundary into Framework. */
static bool valid_descriptor(const UmiDataConnectorDescriptor *descriptor)
{
    return descriptor != NULL && descriptor->connector_id[0] != '\0' &&
        descriptor->display_name[0] != '\0' &&
        descriptor->family >= UMI_DATA_CONNECTOR_RELATIONAL &&
        descriptor->family <= UMI_DATA_CONNECTOR_TIME_SERIES &&
        descriptor->open != NULL && descriptor->execute != NULL &&
        descriptor->close != NULL;
}

/* Validate public connection inputs without ever interpreting credential data. */
static bool valid_open_request(const UmiDataConnectorOpenRequest *request)
{
    return request != NULL && request->endpoint != NULL &&
        request->endpoint[0] != '\0' && request->database_name != NULL &&
        request->database_name[0] != '\0' &&
        request->credential_reference != NULL &&
        request->credential_reference[0] != '\0';
}

/* Convert family values to stable display text without allocating memory. */
const char *umi_data_connector_family_text(UmiDataConnectorFamily family)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (family) {
        case UMI_DATA_CONNECTOR_RELATIONAL: return "relational";
        case UMI_DATA_CONNECTOR_DOCUMENT: return "document";
        case UMI_DATA_CONNECTOR_KEY_VALUE: return "key-value";
        case UMI_DATA_CONNECTOR_GRAPH: return "graph";
        case UMI_DATA_CONNECTOR_COLUMN: return "column";
        case UMI_DATA_CONNECTOR_TIME_SERIES: return "time-series";
        default: return "unknown";
    }
}

/* Allocate checked fixed-capacity descriptor storage. */
UmiStatus umi_data_connector_registry_create(
    size_t connector_capacity,
    UmiDataConnectorRegistry **out_registry)
{
    UmiDataConnectorRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL || connector_capacity == 0U ||
        connector_capacity > SIZE_MAX / sizeof(UmiDataConnectorDescriptor)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = NULL;
    registry = (UmiDataConnectorRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->descriptors = (UmiDataConnectorDescriptor *)calloc(
        connector_capacity, sizeof(*registry->descriptors));
    /* Release owner when child descriptor allocation cannot be completed. */
    if (registry->descriptors == NULL) {
        free(registry);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    registry->capacity = connector_capacity;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/* Release copied descriptors only; driver contexts remain application-owned. */
void umi_data_connector_registry_destroy(UmiDataConnectorRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    free(registry->descriptors);
    free(registry);
}

/* Copy a unique connector descriptor after validating every required callback. */
UmiStatus umi_data_connector_registry_register(
    UmiDataConnectorRegistry *registry,
    const UmiDataConnectorDescriptor *descriptor)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || !valid_descriptor(descriptor)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Duplicate identifiers would make connection selection order-dependent. */
        if (strcmp(registry->descriptors[index].connector_id,
                   descriptor->connector_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= registry->capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->descriptors[registry->count] = *descriptor;
    registry->count += 1U;
    return UMI_STATUS_OK;
}

/* Copy a descriptor by stable connector ID without lending registry memory. */
UmiStatus umi_data_connector_registry_find(
    const UmiDataConnectorRegistry *registry,
    const char *connector_id,
    UmiDataConnectorDescriptor *out_descriptor)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || connector_id == NULL || connector_id[0] == '\0' ||
        out_descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->descriptors[index].connector_id,
                   connector_id) == 0) {
            *out_descriptor = registry->descriptors[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Open provider state and copy callbacks into a self-contained caller handle. */
UmiStatus umi_data_connector_registry_open(
    const UmiDataConnectorRegistry *registry,
    const char *connector_id,
    const UmiDataConnectorOpenRequest *request,
    UmiDataConnectorHandle *out_handle)
{
    UmiDataConnectorDescriptor descriptor;
    void *connection = NULL;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_handle == NULL || !valid_open_request(request)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Clearing first ensures failure never leaves stale callbacks in the handle. */
    (void)memset(out_handle, 0, sizeof(*out_handle));
    status = umi_data_connector_registry_find(
        registry, connector_id, &descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = descriptor.open(request, descriptor.driver_context, &connection);
    /* Successful adapters must return concrete connection state. */
    if (status == UMI_STATUS_OK && connection == NULL) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_handle->connection = connection;
    out_handle->execute = descriptor.execute;
    out_handle->close = descriptor.close;
    out_handle->driver_context = descriptor.driver_context;
    out_handle->open = true;
    return UMI_STATUS_OK;
}

/* Execute only through a complete open handle and clear summary before callback. */
UmiStatus umi_data_connector_handle_execute(
    UmiDataConnectorHandle *handle,
    const UmiDataConnectorQueryRequest *request,
    UmiDataConnectorQuerySummary *out_summary)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (handle == NULL || !handle->open || handle->connection == NULL ||
        handle->execute == NULL || request == NULL ||
        request->query_text == NULL || request->query_text[0] == '\0' ||
        out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_summary, 0, sizeof(*out_summary));
    return handle->execute(handle->connection, request, out_summary,
                           handle->driver_context);
}

/* Close provider state once and erase callbacks to prevent accidental reuse. */
void umi_data_connector_handle_close(UmiDataConnectorHandle *handle)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (handle == NULL) return;
    /* Callback runs only for a complete open handle created by this API. */
    if (handle->open && handle->connection != NULL && handle->close != NULL) {
        handle->close(handle->connection, handle->driver_context);
    }
    (void)memset(handle, 0, sizeof(*handle));
}

/* Return copied descriptor count, treating a NULL registry as empty. */
size_t umi_data_connector_registry_count(
    const UmiDataConnectorRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
