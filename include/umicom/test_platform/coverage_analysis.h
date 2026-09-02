/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/coverage_analysis.h
 *
 * PURPOSE:
 *   Aggregate line and branch coverage by source file, evaluate quality gates
 *   and expose deterministic regression evidence to desktop, web and CI hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_COVERAGE_ANALYSIS_H
#define UMICOM_TEST_PLATFORM_COVERAGE_ANALYSIS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/test_platform/coverage.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_COVERAGE_ANALYSIS_API_VERSION 1U

/**
 * List the named test coverage gate state values accepted by this public contract.
 */
typedef enum UmiTestCoverageGateState {
    UMI_TEST_COVERAGE_GATE_UNKNOWN = 0,
    UMI_TEST_COVERAGE_GATE_PASSED = 1,
    UMI_TEST_COVERAGE_GATE_WARNING = 2,
    UMI_TEST_COVERAGE_GATE_FAILED = 3
} UmiTestCoverageGateState;

/**
 * Represent the test coverage policy data shared with callers of this public contract.
 */
typedef struct UmiTestCoveragePolicy {
    uint32_t struct_size;
    uint32_t api_version;
    uint32_t minimum_line_basis_points;
    uint32_t minimum_branch_basis_points;
    uint64_t maximum_uncovered_lines;
    int fail_on_line_regression;
    int fail_on_branch_regression;
    int allow_empty;
} UmiTestCoveragePolicy;

/**
 * Represent the test coverage baseline data shared with callers of this public contract.
 */
typedef struct UmiTestCoverageBaseline {
    uint32_t struct_size;
    uint32_t api_version;
    uint32_t line_basis_points;
    uint32_t branch_basis_points;
} UmiTestCoverageBaseline;

/**
 * Represent the test coverage file analysis data shared with callers of this public
 * contract.
 */
typedef struct UmiTestCoverageFileAnalysis {
    uint32_t struct_size;
    uint32_t api_version;
    char uri[1024];
    char session_id[128];
    uint64_t lines_total;
    uint64_t lines_covered;
    uint64_t lines_uncovered;
    uint64_t branches_total;
    uint64_t branches_covered;
    uint64_t branches_uncovered;
    uint32_t line_basis_points;
    uint32_t branch_basis_points;
    int32_t line_delta_basis_points;
    int32_t branch_delta_basis_points;
    UmiTestCoverageGateState gate_state;
    uint64_t source_revision;
} UmiTestCoverageFileAnalysis;

/**
 * Represent the test coverage analysis snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestCoverageAnalysisSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char session_id[128];
    size_t file_count;
    size_t passed_file_count;
    size_t warning_file_count;
    size_t failed_file_count;
    uint64_t lines_total;
    uint64_t lines_covered;
    uint64_t branches_total;
    uint64_t branches_covered;
    uint32_t line_basis_points;
    uint32_t branch_basis_points;
    int32_t line_delta_basis_points;
    int32_t branch_delta_basis_points;
    UmiTestCoverageGateState gate_state;
    uint64_t source_revision;
    uint64_t revision;
} UmiTestCoverageAnalysisSnapshot;

/**
 * Represent the test coverage analysis data shared with callers of this public contract.
 */
typedef struct UmiTestCoverageAnalysis UmiTestCoverageAnalysis;

/**
 * Initialise test coverage policy from caller-provided values so later operations receive
 * a known state.
 */
void umi_test_coverage_policy_init(UmiTestCoveragePolicy *policy);
/**
 * Initialise test coverage baseline from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_coverage_baseline_init(UmiTestCoverageBaseline *baseline);
/**
 * Initialise test coverage analysis from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_coverage_analysis_create(
    UmiTestCoverageAnalysis **out_analysis);
/**
 * Release or reset state held by test coverage analysis so the same storage can be reused
 * safely.
 */
void umi_test_coverage_analysis_destroy(UmiTestCoverageAnalysis *analysis);
/**
 * Provide the test coverage analysis build operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_coverage_analysis_build(
    UmiTestCoverageAnalysis *analysis,
    const UmiTestPlatformCoverageRegistry *coverage,
    const char *session_id,
    const UmiTestCoveragePolicy *policy,
    const UmiTestCoverageBaseline *baseline);
/**
 * Find test coverage analysis while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_test_coverage_analysis_at(
    const UmiTestCoverageAnalysis *analysis,
    size_t position,
    UmiTestCoverageFileAnalysis *out_file);
/**
 * Find test coverage analysis while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_test_coverage_analysis_find(
    const UmiTestCoverageAnalysis *analysis,
    const char *uri,
    UmiTestCoverageFileAnalysis *out_file);
/**
 * Provide the test coverage analysis snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_coverage_analysis_snapshot(
    const UmiTestCoverageAnalysis *analysis,
    UmiTestCoverageAnalysisSnapshot *out_snapshot);
/**
 * Return the number of records represented by test coverage analysis without changing
 * their state.
 */
size_t umi_test_coverage_analysis_count(
    const UmiTestCoverageAnalysis *analysis);
/**
 * Provide the test coverage basis points operation used by this module and its client
 * applications.
 */
uint32_t umi_test_coverage_basis_points(uint64_t covered, uint64_t total);
/**
 * Provide the test coverage gate state text operation used by this module and its client
 * applications.
 */
const char *umi_test_coverage_gate_state_text(UmiTestCoverageGateState state);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_TEST_PLATFORM_COVERAGE_ANALYSIS_H */
