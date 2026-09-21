/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/json_test_support.h
 * PURPOSE: Share deterministic metadata test ownership and assertions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CTEST_JSON_TEST_SUPPORT_H
#define UMICOM_CTEST_JSON_TEST_SUPPORT_H
#include "umicom/test_platform/ctest.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); exit(1); } } while (0)
typedef struct JsonTestState {
    UmiTestPlatformItemRegistry *items;
    UmiTestPlatformSuiteRegistry *suites;
    UmiTestPlatformDiscoveryRegistry *discoveries;
    UmiTestPlatformCtestImportOptions options;
} JsonTestState;
void JsonTestCreate(JsonTestState *state);
void JsonTestDestroy(JsonTestState *state);
void JsonTestSeed(JsonTestState *state);
UmiStatus JsonTestImport(JsonTestState *state, const char *json, UmiTestPlatformCtestImportSummary *summary);
uint64_t JsonTestFingerprint(const JsonTestState *state);
void JsonTestItem(const JsonTestState *state, const char *name, UmiTestPlatformItemSnapshot *out);
#endif
