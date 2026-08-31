/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/connector.h
 *
 * PURPOSE:
 *   Define provider-neutral SQL and NoSQL connector contracts that reference
 *   local credentials by identifier and never copy secret values into profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_CONNECTOR_H
#define UMICOM_DATA_CONNECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DATA_CONNECTOR_ID_CAPACITY 64U
#define UMI_DATA_CONNECTOR_NAME_CAPACITY 96U

/* Families cover common structured and non-relational database models. */
typedef enum UmiDataConnectorFamily {
    UMI_DATA_CONNECTOR_RELATIONAL = 0,
    UMI_DATA_CONNECTOR_DOCUMENT = 1,
    UMI_DATA_CONNECTOR_KEY_VALUE = 2,
    UMI_DATA_CONNECTOR_GRAPH = 3,
    UMI_DATA_CONNECTOR_COLUMN = 4,
    UMI_DATA_CONNECTOR_TIME_SERIES = 5
} UmiDataConnectorFamily;

typedef uint64_t UmiDataConnectorCapabilityMask;

#define UMI_DATA_CONNECTOR_CAPABILITY_TRANSACTIONS      (UINT64_C(1) << 0U)
#define UMI_DATA_CONNECTOR_CAPABILITY_PARAMETERS        (UINT64_C(1) << 1U)
#define UMI_DATA_CONNECTOR_CAPABILITY_SCHEMA_DISCOVERY  (UINT64_C(1) << 2U)
#define UMI_DATA_CONNECTOR_CAPABILITY_STREAMING_RESULTS (UINT64_C(1) << 3U)
#define UMI_DATA_CONNECTOR_CAPABILITY_CHANGE_FEED       (UINT64_C(1) << 4U)

/* Open requests borrow text only while the connector's open callback runs. */
typedef struct UmiDataConnectorOpenRequest {
    const char *endpoint;
    const char *database_name;
    const char *credential_reference;
    bool read_only;
} UmiDataConnectorOpenRequest;

/* Query requests are provider text plus portable safety and row-limit policy. */
typedef struct UmiDataConnectorQueryRequest {
    const char *query_text;
    bool read_only;
    size_t row_limit;
} UmiDataConnectorQueryRequest;

/* Query summaries expose counts without imposing a provider-specific row type. */
typedef struct UmiDataConnectorQuerySummary {
    size_t column_count;
    size_t row_count;
    uint64_t affected_rows;
    bool has_more;
} UmiDataConnectorQuerySummary;

/* Open callbacks create provider-owned connection state in out_connection. */
typedef UmiStatus (*UmiDataConnectorOpenFunction)(
    const UmiDataConnectorOpenRequest *request,
    void *driver_context,
    void **out_connection);

/* Execute callbacks operate only on a connection created by the same adapter. */
typedef UmiStatus (*UmiDataConnectorExecuteFunction)(
    void *connection,
    const UmiDataConnectorQueryRequest *request,
    UmiDataConnectorQuerySummary *out_summary,
    void *driver_context);

/* Close callbacks release provider state but never free Framework handles. */
typedef void (*UmiDataConnectorCloseFunction)(
    void *connection,
    void *driver_context);

/* Descriptors are copied by the registry; the driver context remains borrowed. */
typedef struct UmiDataConnectorDescriptor {
    char connector_id[UMI_DATA_CONNECTOR_ID_CAPACITY];
    char display_name[UMI_DATA_CONNECTOR_NAME_CAPACITY];
    UmiDataConnectorFamily family;
    UmiDataConnectorCapabilityMask capabilities;
    UmiDataConnectorOpenFunction open;
    UmiDataConnectorExecuteFunction execute;
    UmiDataConnectorCloseFunction close;
    void *driver_context;
} UmiDataConnectorDescriptor;

/* A handle copies callbacks so it never points into registry descriptor storage. */
typedef struct UmiDataConnectorHandle {
    void *connection;
    UmiDataConnectorExecuteFunction execute;
    UmiDataConnectorCloseFunction close;
    void *driver_context;
    bool open;
} UmiDataConnectorHandle;

/* The opaque registry owns copied descriptors, not driver contexts. */
typedef struct UmiDataConnectorRegistry UmiDataConnectorRegistry;

/* Convert a connector family into stable UI and diagnostic text. */
const char *umi_data_connector_family_text(UmiDataConnectorFamily family);

/* Create a bounded registry for SQL and NoSQL adapters. */
UmiStatus umi_data_connector_registry_create(
    size_t connector_capacity,
    UmiDataConnectorRegistry **out_registry);

/* Destroy copied descriptors without touching application-owned drivers. */
void umi_data_connector_registry_destroy(UmiDataConnectorRegistry *registry);

/* Register one unique copied connector descriptor. */
UmiStatus umi_data_connector_registry_register(
    UmiDataConnectorRegistry *registry,
    const UmiDataConnectorDescriptor *descriptor);

/* Copy a connector descriptor by stable identifier. */
UmiStatus umi_data_connector_registry_find(
    const UmiDataConnectorRegistry *registry,
    const char *connector_id,
    UmiDataConnectorDescriptor *out_descriptor);

/* Open a provider connection using only a local credential reference. */
UmiStatus umi_data_connector_registry_open(
    const UmiDataConnectorRegistry *registry,
    const char *connector_id,
    const UmiDataConnectorOpenRequest *request,
    UmiDataConnectorHandle *out_handle);

/* Execute a query through callbacks copied into an open handle. */
UmiStatus umi_data_connector_handle_execute(
    UmiDataConnectorHandle *handle,
    const UmiDataConnectorQueryRequest *request,
    UmiDataConnectorQuerySummary *out_summary);

/* Close provider state and clear every callback from the handle. */
void umi_data_connector_handle_close(UmiDataConnectorHandle *handle);

/* Return the number of copied connector descriptors. */
size_t umi_data_connector_registry_count(
    const UmiDataConnectorRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
