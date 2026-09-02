/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/request_test_support.h
 *
 * PURPOSE:
 *   Provide a deterministic memory-transport fixture for all LSP request tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_TEST_SUPPORT_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_TEST_SUPPORT_H
#include "umicom/language_runtime/memory_transport.h"
#include "umicom/language_runtime/server.h"
/**
 * Represent the request test fixture data shared with callers of this public contract.
 */
typedef struct RequestTestFixture{UmiLanguageRuntimeMemoryTransport*memory;UmiLanguageRuntimeServer*server;}RequestTestFixture;
/**
 * Initialise request test fixture from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus request_test_fixture_create(RequestTestFixture*f);
/**
 * Release or reset state held by request test fixture so the same storage can be reused
 * safely.
 */
void request_test_fixture_destroy(RequestTestFixture*f);
#endif
