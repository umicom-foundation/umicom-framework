/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/request_test_support.h
 *
 * PURPOSE:
 *   Shared deterministic request fixture using the bounded memory transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_TEST_SUPPORT_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_TEST_SUPPORT_H
#include "umicom/debug_runtime/adapter.h"

/**
 * Represent the debug request test fixture data shared with callers of this public
 * contract.
 */
typedef struct DebugRequestTestFixture {
    UmiDebugRuntimeMemoryTransport *memory;
    UmiDebugRuntimeAdapter *adapter;
} DebugRequestTestFixture;

/**
 * Initialise debug request test fixture from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus debug_request_test_fixture_create(
    DebugRequestTestFixture *fixture);
/**
 * Release or reset state held by debug request test fixture so the same storage can be
 * reused safely.
 */
void debug_request_test_fixture_destroy(
    DebugRequestTestFixture *fixture);
/**
 * Exercise debug request test fixture written and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus debug_request_test_fixture_written(
    DebugRequestTestFixture *fixture,
    char *out_text,
    size_t capacity);

#endif
