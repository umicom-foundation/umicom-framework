/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_adapter.h
 *
 * PURPOSE:
 *   Discover and execute CTest suites through the portable Framework test model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_CTEST_ADAPTER_H
#define UMICOM_TESTING_CTEST_ADAPTER_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/testing/runner.h"
#include "umicom/testing/suite.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ctest discover operation used by this module and its client applications.
 */
UmiStatus umi_ctest_discover(const char *build_directory,
                             UmiTestSuite *suite,
                             size_t *out_discovered);
/**
 * Perform ctest through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_ctest_run(const char *build_directory,
                        const char *test_name,
                        UmiTestResult *out_result);

/** Discovery policy. Zero-initialise before use. An absent configuration
 * preserves CTest's default selection. A zero timeout selects a 30-second
 * discovery deadline. Cancellation is borrowed for the synchronous call.
 */
typedef struct UmiCtestDiscoveryOptions {
    const char *configuration;
    const UmiCancellationToken *cancellation;
    uint32_t timeout_ms;
} UmiCtestDiscoveryOptions;

/** Discover a complete bounded legacy catalogue without partial publication.
 * Uses argv, captures process truncation and requires a matching Total Tests
 * footer. Failed launch, timeout, cancellation, malformed output, duplicate
 * identities or capacity exhaustion leave suite unchanged and count zero.
 * The build root must contain a readable CTestTestfile.cmake.
 * outDiagnostics may be NULL only when diagnosticsCapacity is zero. Otherwise
 * it receives a short actionable status message. The JSON metadata provider
 * has a separate contract and is not qualified or enlarged by this function.
 * This call does not raise the 1,024-case / 64-KiB capture limits. It never runs
 * test bodies. Input and output storage must not overlap.
 */
UmiStatus UmiCtestDiscoverConfigured(const char *buildDirectory,
    const UmiCtestDiscoveryOptions *options, UmiTestSuite *suite,
    size_t *outDiscovered, char *outDiagnostics, size_t diagnosticsCapacity);

/** Maximum accepted CTest JUnit document size, excluding a trailing NUL. */
#define UMI_CTEST_REPORT_MAX_BYTES (8U * 1024U * 1024U)
/** An anchored pattern may need two bytes per literal test-name byte. */
#define UMI_CTEST_EXACT_PATTERN_CAPACITY (2U * UMI_TEST_NAME_CAPACITY + 3U)

/** Optional execution policy. Initialise the whole structure to zero first.
 * configuration == NULL or "" leaves CTest's configuration selection unchanged.
 * test_id == NULL uses test_name as the legacy result identifier. An explicit
 * item ID lets Test Explorer retain a long display name without truncating IDs.
 * timeout_ms limits the complete CTest invocation, including automatic fixtures;
 * zero leaves individual test timeouts to CTest. Cancellation is borrowed only
 * for the synchronous call. This contract never retains any input pointer.
 */
typedef struct UmiCtestRunOptions {
    const char *configuration;
    const char *test_id;
    const UmiCancellationToken *cancellation;
    uint32_t timeout_ms;
} UmiCtestRunOptions;

/** Facts from the one exactly named testcase, not the CTest process summary.
 * testcase_count includes setup/cleanup fixtures that CTest added. duration_ms
 * is the selected testcase's measured JUnit time, truncated to milliseconds.
 * A disabled test is SKIPPED; an unavailable executable is NOT_RUN.
 */
typedef struct UmiCtestReportSnapshot {
    UmiTestState state;
    uint64_t duration_ms;
    size_t testcase_count;
    char message[256];
} UmiCtestReportSnapshot;

/** Turn one literal printable UTF-8 test name into an anchored CTest expression.
 * Input must fit UMI_TEST_NAME_CAPACITY. Failure leaves outPattern unchanged.
 * This is regular-expression escaping, not shell quoting: execution uses argv.
 */
UmiStatus UmiCtestEscapeName(const char *testName, char *outPattern,
    size_t capacity);

/** Read the bounded CTest --output-junit subset, not arbitrary third-party XML.
 * The report must have one testsuite root and exactly one matching testcase.
 * The declared testcase count must agree with the document. Unknown outcome
 * encodings, malformed nesting, duplicate fields, DTDs and external entities
 * fail explicitly. Comments and CDATA cannot masquerade as testcases.
 * Failure leaves outSnapshot unchanged. No filesystem or process is accessed.
 */
UmiStatus UmiCtestParseReport(const char *xml, size_t length,
    const char *testName, UmiCtestReportSnapshot *outSnapshot);

/** Execute the exactly named test from the CTest BUILD ROOT, not its test-level
 * WORKING_DIRECTORY. Requires CTest 3.21+ for --output-junit. A fresh private
 * report directory is owned for this call and is cleaned on every return path.
 * Invalid input, missing/malformed reports and missing tests never become PASS.
 * The return status describes execution/reporting failure. A legitimate SKIP
 * can return OK; call UmiCtestResultRequirePassed for a strict acceptance gate.
 * Output is a bounded diagnostic tail, with an explicit truncation notice.
 * The existing umi_ctest_run delegates here with default options.
 */
UmiStatus UmiCtestRunConfigured(const char *buildDirectory,
    const char *testName, const UmiCtestRunOptions *options,
    UmiTestResult *outResult);

/** Run an owned CTest catalogue in its discovery build root. This is not a
 * replacement for the generic runner: only CTest catalogues belong here.
 * Every case keeps its own ID; options.test_id is ignored. A nonzero case
 * timeout overrides options.timeout_ms for that complete invocation.
 * Disabled cases remain SKIPPED without a launch. Cancellation prevents later
 * launches and still fills every result slot, matching the generic suite API.
 * NOT_RUN counts as failed in the aggregate because UmiTestRunSummary has no
 * not-run field. Return the first execution error, retaining all case results.
 * A successful call can contain legitimate skips; it is not an all-passed gate.
 */
UmiStatus UmiCtestRunSuite(const char *buildDirectory, const UmiTestSuite *suite,
    const UmiCtestRunOptions *options, UmiTestResult *results,
    size_t resultCapacity, UmiTestRunSummary *outSummary);

/** Accept only a passed testcase, successful process and successful report.
 * Skipped, disabled and not-run results return INVALID_STATE, not success.
 */
UmiStatus UmiCtestResultRequirePassed(const UmiTestResult *result);

#ifdef __cplusplus
}
#endif

#endif
