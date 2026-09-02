/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/request_test_support.c
 *
 * PURPOSE:
 *   Implement the memory-transport DAP request fixture.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "request_test_support.h"

#include <string.h>

/*
 * Initialise debug request test fixture from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus debug_request_test_fixture_create(
    DebugRequestTestFixture *fixture)
{
    UmiDebugRuntimeTransport transport;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fixture == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    (void)memset(fixture, 0, sizeof(*fixture));
    (void)memset(&transport, 0, sizeof(transport));

    status = umi_debug_runtime_memory_transport_create(
        &fixture->memory,
        &transport);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_create_with_transport(
        "test.adapter",
        &transport,
        &fixture->adapter);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (transport.instance != NULL && transport.destroy != NULL) {
            transport.destroy(transport.instance);
        }
        fixture->memory = NULL;
    }

    return status;
}

/*
 * Release or reset state held by debug request test fixture so the same storage can be
 * reused safely.
 */
void debug_request_test_fixture_destroy(
    DebugRequestTestFixture *fixture)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fixture == NULL) return;
    umi_debug_runtime_adapter_destroy(fixture->adapter);
    fixture->adapter = NULL;
    fixture->memory = NULL;
}

/*
 * Exercise debug request test fixture written and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus debug_request_test_fixture_written(
    DebugRequestTestFixture *fixture,
    char *out_text,
    size_t capacity)
{
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fixture == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    return umi_debug_runtime_memory_transport_written(
        fixture->memory,
        out_text,
        capacity,
        &count);
}
