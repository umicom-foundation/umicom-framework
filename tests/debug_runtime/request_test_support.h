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

typedef struct DebugRequestTestFixture {
    UmiDebugRuntimeMemoryTransport *memory;
    UmiDebugRuntimeAdapter *adapter;
} DebugRequestTestFixture;

UmiStatus debug_request_test_fixture_create(
    DebugRequestTestFixture *fixture);
void debug_request_test_fixture_destroy(
    DebugRequestTestFixture *fixture);
UmiStatus debug_request_test_fixture_written(
    DebugRequestTestFixture *fixture,
    char *out_text,
    size_t capacity);

#endif
