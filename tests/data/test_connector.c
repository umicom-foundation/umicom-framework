/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data/test_connector.c
 *
 * PURPOSE:
 *   Verify provider-neutral connector registration, execution and close safety.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "umicom/data/connector.h"

/* Open fake provider state only when a local credential reference is supplied. */
static UmiStatus fake_open(
    const UmiDataConnectorOpenRequest *request,
    void *driver_context,
    void **out_connection)
{
    int *connection;
    (void)driver_context;
    if (request == NULL || out_connection == NULL ||
        request->credential_reference == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    connection = (int *)calloc(1U, sizeof(*connection));
    if (connection == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *connection = 42;
    *out_connection = connection;
    return UMI_STATUS_OK;
}

/* Return deterministic summary counts without exposing provider row types. */
static UmiStatus fake_execute(
    void *connection,
    const UmiDataConnectorQueryRequest *request,
    UmiDataConnectorQuerySummary *out_summary,
    void *driver_context)
{
    (void)driver_context;
    if (connection == NULL || request == NULL || out_summary == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_summary->column_count = 2U;
    out_summary->row_count = request->row_limit < 3U ? request->row_limit : 3U;
    return UMI_STATUS_OK;
}

/* Release only the provider connection allocated by fake_open. */
static void fake_close(void *connection, void *driver_context)
{
    (void)driver_context;
    free(connection);
}

/* Exercise descriptor copying and a complete safe handle lifecycle. */
int main(void)
{
    UmiDataConnectorRegistry *registry = NULL;
    UmiDataConnectorDescriptor descriptor = {0};
    UmiDataConnectorHandle handle = {0};
    UmiDataConnectorOpenRequest open_request = {
        "localhost", "examples", "secret://database/test", true};
    UmiDataConnectorQueryRequest query = {
        "select id, name from examples", true, 10U};
    UmiDataConnectorQuerySummary summary;

    /* The registry copies callbacks and metadata, never raw credential values. */
    assert(umi_data_connector_registry_create(2U, &registry) == UMI_STATUS_OK);
    (void)snprintf(descriptor.connector_id,
                   sizeof(descriptor.connector_id), "%s", "test.sql");
    (void)snprintf(descriptor.display_name,
                   sizeof(descriptor.display_name), "%s", "Test SQL");
    descriptor.family = UMI_DATA_CONNECTOR_RELATIONAL;
    descriptor.capabilities = UMI_DATA_CONNECTOR_CAPABILITY_TRANSACTIONS;
    descriptor.open = fake_open;
    descriptor.execute = fake_execute;
    descriptor.close = fake_close;
    assert(umi_data_connector_registry_register(registry, &descriptor) ==
           UMI_STATUS_OK);

    /* An open handle remains usable even though it does not point at the registry. */
    assert(umi_data_connector_registry_open(
        registry, "test.sql", &open_request, &handle) == UMI_STATUS_OK);
    assert(umi_data_connector_handle_execute(
        &handle, &query, &summary) == UMI_STATUS_OK);
    assert(summary.column_count == 2U);
    assert(summary.row_count == 3U);
    umi_data_connector_handle_close(&handle);
    assert(!handle.open);

    umi_data_connector_registry_destroy(registry);
    return 0;
}
