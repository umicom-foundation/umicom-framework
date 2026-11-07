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
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/data/connector.h"

/* Open fake provider state only when a local credential reference is supplied. */
static UmiStatus fake_open(
    const UmiDataConnectorOpenRequest *request,
    void *driver_context,
    void **out_connection)
{
    int *connection;
    (void)driver_context;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || out_connection == NULL ||
        request->credential_reference == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    connection = (int *)calloc(1U, sizeof(*connection));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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


/* The callback deliberately re-enters close without using a freed connection.
 * A second callback invocation fails before free, making the ownership defect
 * visible without relying on an allocator crash. */
typedef struct ReentrantCloseFixture {
    UmiDataConnectorHandle *handle;
    unsigned calls;
} ReentrantCloseFixture;

static void reentrant_close(void *connection, void *driver_context)
{
    ReentrantCloseFixture *fixture = driver_context;
    fixture->calls += 1U;
    assert(fixture->calls == 1U);
    assert(!fixture->handle->open);
    assert(fixture->handle->connection == NULL);
    assert(fixture->handle->execute == NULL);
    assert(fixture->handle->close == NULL);
    assert(fixture->handle->driver_context == NULL);
    umi_data_connector_handle_close(fixture->handle);
    free(connection);
}

static void check_descriptor_bounds_and_close(void)
{
    UmiDataConnectorRegistry *registry = NULL;
    UmiDataConnectorDescriptor descriptor = {0};
    UmiDataConnectorDescriptor invalid;
    UmiDataConnectorHandle handle = {0};
    ReentrantCloseFixture fixture = {&handle, 0U};
    UmiDataConnectorOpenRequest request = {
        "localhost", "lesson", "secret://lesson/fixture", true};
    UmiDataConnectorQueryRequest query = {"fixture", true, 2U};
    UmiDataConnectorQuerySummary summary;

    assert(umi_data_connector_registry_create(2U, &registry) == UMI_STATUS_OK);
    (void)memset(descriptor.connector_id, 'i', sizeof(descriptor.connector_id) - 1U);
    (void)memset(descriptor.display_name, 'n', sizeof(descriptor.display_name) - 1U);
    descriptor.family = UMI_DATA_CONNECTOR_DOCUMENT;
    descriptor.open = fake_open;
    descriptor.execute = fake_execute;
    descriptor.close = reentrant_close;
    descriptor.driver_context = &fixture;
    invalid = descriptor;
    invalid.connector_id[sizeof(invalid.connector_id) - 1U] = 'i';
    assert(umi_data_connector_registry_register(registry, &invalid) ==
           UMI_STATUS_INVALID_ARGUMENT);
    invalid = descriptor;
    invalid.display_name[sizeof(invalid.display_name) - 1U] = 'n';
    assert(umi_data_connector_registry_register(registry, &invalid) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_data_connector_registry_count(registry) == 0U);
    assert(umi_data_connector_registry_register(registry, &descriptor) == UMI_STATUS_OK);
    assert(umi_data_connector_registry_open(
        registry, descriptor.connector_id, &request, &handle) == UMI_STATUS_OK);

    /* Copied callbacks survive registry destruction while the separate driver
     * context and handle remain alive on this stack. */
    umi_data_connector_registry_destroy(registry);
    assert(umi_data_connector_handle_execute(&handle, &query, &summary) == UMI_STATUS_OK);
    assert(summary.row_count == 2U);
    umi_data_connector_handle_close(&handle);
    umi_data_connector_handle_close(&handle);
    umi_data_connector_handle_close(NULL);
    assert(fixture.calls == 1U);
    assert(umi_data_connector_handle_execute(&handle, &query, &summary) ==
           UMI_STATUS_INVALID_ARGUMENT);
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
    check_descriptor_bounds_and_close();
    return 0;
}
