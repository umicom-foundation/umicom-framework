/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/request_test_support.c
 *
 * PURPOSE:
 *   Implement the memory-transport DAP request fixture.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "request_test_support.h"

#include <string.h>

UmiStatus debug_request_test_fixture_create(
    DebugRequestTestFixture *fixture)
{
    UmiDebugRuntimeTransport transport;
    UmiStatus status;

    if (fixture == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    (void)memset(fixture, 0, sizeof(*fixture));
    (void)memset(&transport, 0, sizeof(transport));

    status = umi_debug_runtime_memory_transport_create(
        &fixture->memory,
        &transport);
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_create_with_transport(
        "test.adapter",
        &transport,
        &fixture->adapter);

    if (status != UMI_STATUS_OK) {
        if (transport.instance != NULL && transport.destroy != NULL) {
            transport.destroy(transport.instance);
        }
        fixture->memory = NULL;
    }

    return status;
}

void debug_request_test_fixture_destroy(
    DebugRequestTestFixture *fixture)
{
    if (fixture == NULL) return;
    umi_debug_runtime_adapter_destroy(fixture->adapter);
    fixture->adapter = NULL;
    fixture->memory = NULL;
}

UmiStatus debug_request_test_fixture_written(
    DebugRequestTestFixture *fixture,
    char *out_text,
    size_t capacity)
{
    size_t count = 0U;

    if (fixture == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    return umi_debug_runtime_memory_transport_written(
        fixture->memory,
        out_text,
        capacity,
        &count);
}
