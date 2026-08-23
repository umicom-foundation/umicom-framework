/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/request_test_support.h
 *
 * PURPOSE:
 *   Provide a deterministic memory-transport fixture for all LSP request tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_TEST_SUPPORT_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_TEST_SUPPORT_H
#include "umicom/language_runtime/memory_transport.h"
#include "umicom/language_runtime/server.h"
typedef struct RequestTestFixture{UmiLanguageRuntimeMemoryTransport*memory;UmiLanguageRuntimeServer*server;}RequestTestFixture;
UmiStatus request_test_fixture_create(RequestTestFixture*f);
void request_test_fixture_destroy(RequestTestFixture*f);
#endif
