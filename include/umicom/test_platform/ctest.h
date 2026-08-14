/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/ctest.h
 *
 * PURPOSE:
 *   Import CTest JSON v1 discovery into Framework-owned suites, hierarchy and
 *   metadata records without leaking CMake or JSON parser types to frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_CTEST_H
#define UMICOM_TEST_PLATFORM_CTEST_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/discovery.h"
#include "umicom/test_platform/item.h"
#include "umicom/test_platform/suite.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestPlatformCtestImportOptions {
    char project_id[128];
    char suite_id[128];
    char configuration[128];
    char build_directory[1024];
} UmiTestPlatformCtestImportOptions;

typedef struct UmiTestPlatformCtestImportSummary {
    size_t discovered_count;
    size_t disabled_count;
    size_t labelled_count;
    size_t timed_count;
} UmiTestPlatformCtestImportSummary;

UmiStatus umi_test_platform_ctest_parse_json(
    const char *json,
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformItemRegistry *items,
    UmiTestPlatformSuiteRegistry *suites,
    UmiTestPlatformDiscoveryRegistry *discoveries,
    UmiTestPlatformCtestImportSummary *out_summary
);
/* Invoke ctest with argument arrays, capture diagnostics, then use the same
 * parser exercised by deterministic unit tests. */
UmiStatus umi_test_platform_ctest_discover(
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformItemRegistry *items,
    UmiTestPlatformSuiteRegistry *suites,
    UmiTestPlatformDiscoveryRegistry *discoveries,
    UmiTestPlatformCtestImportSummary *out_summary,
    char *out_diagnostics,
    size_t diagnostics_capacity
);

#ifdef __cplusplus
}
#endif
#endif
