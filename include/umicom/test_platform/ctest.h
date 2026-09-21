/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/ctest.h
 *
 * PURPOSE:
 *   Import CTest JSON v1 discovery into Framework-owned suites, hierarchy and
 *   metadata records without leaking CMake or JSON parser types to frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_CTEST_H
#define UMICOM_TEST_PLATFORM_CTEST_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#include "umicom/test_platform/discovery.h"
#include "umicom/test_platform/item.h"
#include "umicom/test_platform/suite.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test platform ctest import options data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformCtestImportOptions {
    char project_id[128];
    char suite_id[128];
    char configuration[128];
    char build_directory[1024];
} UmiTestPlatformCtestImportOptions;

/**
 * Represent the test platform ctest import summary data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformCtestImportSummary {
    size_t discovered_count;
    size_t disabled_count;
    size_t labelled_count;
    size_t timed_count;
} UmiTestPlatformCtestImportSummary;

/**
 * Provide the test platform ctest parse json operation used by this module and its client
 * applications.
 */
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

/* Live capture grows independently of the retained 64 KiB process diagnostic
 * tail. Limits include the entire JSON document and every published root item. */
#define UMI_TEST_PLATFORM_CTEST_DEFAULT_BYTES (32U * 1024U * 1024U)
#define UMI_TEST_PLATFORM_CTEST_MAX_BYTES (128U * 1024U * 1024U)
#define UMI_TEST_PLATFORM_CTEST_DEFAULT_TIMEOUT_MS 120000U

typedef struct UmiTestPlatformCtestDiscoveryOptions {
    size_t maximum_output_bytes;  /* zero selects the 32 MiB default */
    size_t maximum_tests;         /* zero selects registry hard limit minus root */
    uint32_t timeout_ms;          /* zero selects the bounded default */
    const UmiCancellationToken *cancellation; /* borrowed for this call */
} UmiTestPlatformCtestDiscoveryOptions;

/** Parse a complete byte span with a required ctestInfo/version-1 envelope.
 * Reject embedded NULs, malformed UTF-8, unsupported versions, truncation and
 * unrepresentable metadata. All three registries are replaced transactionally
 * for options->suite_id; unrelated suites survive. A failed import leaves all
 * records and revisions untouched and zeroes outSummary when supplied.
 *
 * Registry access must be serialised by the owner. Publication performs no
 * allocation or callbacks, but is not a concurrent-reader transaction.
 * The legacy string parser also accepts envelope-free {"tests": [...]} fixtures
 * and historical version objects containing only major: 1.
 */
UmiStatus UmiTestPlatformCtestParseJsonSized(const char *json, size_t length,
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformItemRegistry *items, UmiTestPlatformSuiteRegistry *suites,
    UmiTestPlatformDiscoveryRegistry *discoveries,
    UmiTestPlatformCtestImportSummary *outSummary);

/** Run the existing JSON metadata provider with bounded raw process capture.
 * This lists tests; it does not run the test commands. CTest itself reads the
 * configured build's scripts, so use a trusted build root. A readable root
 * CTestTestfile.cmake is required, including for an intentionally empty suite.
 * An empty valid JSON catalogue removes obsolete tests in this suite.
 * Combined stderr/stdout contamination is an error, never silently discarded.
 */
UmiStatus UmiTestPlatformCtestDiscoverConfigured(
    const UmiTestPlatformCtestImportOptions *options,
    const UmiTestPlatformCtestDiscoveryOptions *execution,
    UmiTestPlatformItemRegistry *items, UmiTestPlatformSuiteRegistry *suites,
    UmiTestPlatformDiscoveryRegistry *discoveries,
    UmiTestPlatformCtestImportSummary *outSummary,
    char *outDiagnostics, size_t diagnosticsCapacity);

#ifdef __cplusplus
}
#endif
#endif
