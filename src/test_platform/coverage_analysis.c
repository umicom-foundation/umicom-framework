/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/coverage_analysis.c
 *
 * PURPOSE:
 *   Implement file-level and aggregate coverage analysis with integer basis
 *   points. Integer ratios avoid platform-specific floating rounding in gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/coverage_analysis.h"

#include <stdlib.h>
#include <string.h>

#define COVERAGE_FILE_CAPACITY UMI_TEST_PLATFORM_COVERAGE_CAPACITY

struct UmiTestCoverageAnalysis {
    UmiTestCoverageFileAnalysis *files;
    size_t count;
    UmiTestCoverageAnalysisSnapshot snapshot;
    uint64_t revision;
};

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Provide the test coverage basis points operation used by this module and its client
 * applications.
 */
uint32_t umi_test_coverage_basis_points(uint64_t covered, uint64_t total)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (total == 0U) return 0U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (covered >= total) return 10000U;
    /* Split the multiplication to avoid overflow for very large reports. */
    return (uint32_t)(((covered / total) * 10000U) +
                      (((covered % total) * 10000U) / total));
}

/*
 * Initialise test coverage policy from caller-provided values so later operations receive
 * a known state.
 */
void umi_test_coverage_policy_init(UmiTestCoveragePolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->struct_size = (uint32_t)sizeof(*policy);
    policy->api_version = UMI_TEST_COVERAGE_ANALYSIS_API_VERSION;
    policy->minimum_line_basis_points = 8000U;
    policy->minimum_branch_basis_points = 7000U;
    policy->maximum_uncovered_lines = UINT64_MAX;
    policy->fail_on_line_regression = 1;
    policy->fail_on_branch_regression = 1;
}

/*
 * Initialise test coverage baseline from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_coverage_baseline_init(UmiTestCoverageBaseline *baseline)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (baseline == NULL) return;
    (void)memset(baseline, 0, sizeof(*baseline));
    baseline->struct_size = (uint32_t)sizeof(*baseline);
    baseline->api_version = UMI_TEST_COVERAGE_ANALYSIS_API_VERSION;
}

/*
 * Initialise test coverage analysis from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_coverage_analysis_create(
    UmiTestCoverageAnalysis **out_analysis)
{
    UmiTestCoverageAnalysis *analysis;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_analysis == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_analysis = NULL;
    analysis = (UmiTestCoverageAnalysis *)calloc(1U, sizeof(*analysis));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    analysis->files = (UmiTestCoverageFileAnalysis *)calloc(
        COVERAGE_FILE_CAPACITY, sizeof(analysis->files[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis->files == NULL) {
        free(analysis);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    analysis->revision = 1U;
    *out_analysis = analysis;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by test coverage analysis so the same storage can be reused
 * safely.
 */
void umi_test_coverage_analysis_destroy(UmiTestCoverageAnalysis *analysis)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL) return;
    free(analysis->files);
    free(analysis);
}

/* Provide the find file operation used by this module and its client applications. */
static size_t find_file(const UmiTestCoverageAnalysis *analysis,
                        const char *uri)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < analysis->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(analysis->files[index].uri, uri) == 0) return index;
    }
    return (size_t)-1;
}

/* Provide the compare file operation used by this module and its client applications. */
static int compare_file(const void *left_value, const void *right_value)
{
    const UmiTestCoverageFileAnalysis *left =
        (const UmiTestCoverageFileAnalysis *)left_value;
    const UmiTestCoverageFileAnalysis *right =
        (const UmiTestCoverageFileAnalysis *)right_value;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->gate_state > right->gate_state) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->gate_state < right->gate_state) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->line_basis_points < right->line_basis_points) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->line_basis_points > right->line_basis_points) return 1;
    return strcmp(left->uri, right->uri);
}

/* Provide the evaluate gate operation used by this module and its client applications. */
static UmiTestCoverageGateState evaluate_gate(
    uint64_t lines_total,
    uint64_t lines_uncovered,
    uint32_t line_basis_points,
    uint32_t branch_basis_points,
    int32_t line_delta,
    int32_t branch_delta,
    const UmiTestCoveragePolicy *policy)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (lines_total == 0U) {
        return policy->allow_empty ? UMI_TEST_COVERAGE_GATE_PASSED
                                   : UMI_TEST_COVERAGE_GATE_UNKNOWN;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (line_basis_points < policy->minimum_line_basis_points ||
        lines_uncovered > policy->maximum_uncovered_lines ||
        (policy->fail_on_line_regression && line_delta < 0) ||
        (policy->fail_on_branch_regression && branch_delta < 0)) {
        return UMI_TEST_COVERAGE_GATE_FAILED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (branch_basis_points < policy->minimum_branch_basis_points) {
        return UMI_TEST_COVERAGE_GATE_WARNING;
    }
    return UMI_TEST_COVERAGE_GATE_PASSED;
}

/* Provide the finalise file operation used by this module and its client applications. */
static void finalise_file(UmiTestCoverageFileAnalysis *file,
                          const UmiTestCoveragePolicy *policy,
                          const UmiTestCoverageBaseline *baseline)
{
    file->lines_uncovered = file->lines_total >= file->lines_covered
        ? file->lines_total - file->lines_covered
        : 0U;
    file->branches_uncovered = file->branches_total >= file->branches_covered
        ? file->branches_total - file->branches_covered
        : 0U;
    file->line_basis_points = umi_test_coverage_basis_points(
        file->lines_covered, file->lines_total);
    file->branch_basis_points = umi_test_coverage_basis_points(
        file->branches_covered, file->branches_total);
    file->line_delta_basis_points =
        (int32_t)file->line_basis_points - (int32_t)baseline->line_basis_points;
    file->branch_delta_basis_points =
        (int32_t)file->branch_basis_points -
        (int32_t)baseline->branch_basis_points;
    file->gate_state = evaluate_gate(
        file->lines_total, file->lines_uncovered, file->line_basis_points,
        file->branch_basis_points, file->line_delta_basis_points,
        file->branch_delta_basis_points, policy);
}

/*
 * Provide the test coverage analysis build operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_coverage_analysis_build(
    UmiTestCoverageAnalysis *analysis,
    const UmiTestPlatformCoverageRegistry *coverage,
    const char *session_id,
    const UmiTestCoveragePolicy *policy,
    const UmiTestCoverageBaseline *baseline)
{
    size_t index;
    UmiTestCoveragePolicy effective_policy;
    UmiTestCoverageBaseline effective_baseline;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL || coverage == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) {
        umi_test_coverage_policy_init(&effective_policy);
        policy = &effective_policy;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (baseline == NULL) {
        umi_test_coverage_baseline_init(&effective_baseline);
        baseline = &effective_baseline;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->struct_size != (uint32_t)sizeof(*policy) ||
        policy->api_version != UMI_TEST_COVERAGE_ANALYSIS_API_VERSION ||
        baseline->struct_size != (uint32_t)sizeof(*baseline) ||
        baseline->api_version != UMI_TEST_COVERAGE_ANALYSIS_API_VERSION ||
        policy->minimum_line_basis_points > 10000U ||
        policy->minimum_branch_basis_points > 10000U ||
        baseline->line_basis_points > 10000U ||
        baseline->branch_basis_points > 10000U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    analysis->count = 0U;
    (void)memset(&analysis->snapshot, 0, sizeof(analysis->snapshot));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_test_platform_coverage_registry_count(coverage);
         ++index) {
        UmiTestPlatformCoverageSnapshot snapshot;
        size_t file_index;
        UmiStatus status = umi_test_platform_coverage_registry_at(
            coverage, index, &snapshot);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (session_id != NULL && session_id[0] != '\0' &&
            strcmp(snapshot.session_id, session_id) != 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (snapshot.uri[0] == '\0') continue;
        file_index = find_file(analysis, snapshot.uri);
        /* Apply this branch only when its contract condition is satisfied. */
        if (file_index == (size_t)-1) {
            UmiTestCoverageFileAnalysis *file;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (analysis->count >= COVERAGE_FILE_CAPACITY) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            file_index = analysis->count++;
            file = &analysis->files[file_index];
            (void)memset(file, 0, sizeof(*file));
            file->struct_size = (uint32_t)sizeof(*file);
            file->api_version = UMI_TEST_COVERAGE_ANALYSIS_API_VERSION;
            copy_text(file->uri, sizeof(file->uri), snapshot.uri);
            copy_text(file->session_id, sizeof(file->session_id),
                      snapshot.session_id);
        }
        /* Multiple instrumentation fragments for the same URI and session are
         * additive. If providers emit revisions of one record, callers should
         * use stable ids and replace them in the canonical registry first. */
        analysis->files[file_index].lines_total += snapshot.lines_total;
        analysis->files[file_index].lines_covered += snapshot.lines_covered;
        analysis->files[file_index].branches_total += snapshot.branches_total;
        analysis->files[file_index].branches_covered += snapshot.branches_covered;
        /* Apply this branch only when its contract condition is satisfied. */
        if (snapshot.revision > analysis->files[file_index].source_revision) {
            analysis->files[file_index].source_revision = snapshot.revision;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < analysis->count; ++index) {
        finalise_file(&analysis->files[index], policy, baseline);
    }
    qsort(analysis->files, analysis->count, sizeof(analysis->files[0]),
          compare_file);

    analysis->snapshot.struct_size = (uint32_t)sizeof(analysis->snapshot);
    analysis->snapshot.api_version = UMI_TEST_COVERAGE_ANALYSIS_API_VERSION;
    copy_text(analysis->snapshot.session_id,
              sizeof(analysis->snapshot.session_id), session_id);
    analysis->snapshot.file_count = analysis->count;
    analysis->snapshot.source_revision =
        umi_test_platform_coverage_registry_revision(coverage);
    analysis->snapshot.gate_state = analysis->count == 0U
        ? (policy->allow_empty ? UMI_TEST_COVERAGE_GATE_PASSED
                               : UMI_TEST_COVERAGE_GATE_UNKNOWN)
        : UMI_TEST_COVERAGE_GATE_PASSED;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < analysis->count; ++index) {
        const UmiTestCoverageFileAnalysis *file = &analysis->files[index];
        analysis->snapshot.lines_total += file->lines_total;
        analysis->snapshot.lines_covered += file->lines_covered;
        analysis->snapshot.branches_total += file->branches_total;
        analysis->snapshot.branches_covered += file->branches_covered;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (file->gate_state == UMI_TEST_COVERAGE_GATE_FAILED) {
            ++analysis->snapshot.failed_file_count;
            analysis->snapshot.gate_state = UMI_TEST_COVERAGE_GATE_FAILED;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (file->gate_state == UMI_TEST_COVERAGE_GATE_WARNING) {
            ++analysis->snapshot.warning_file_count;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (analysis->snapshot.gate_state != UMI_TEST_COVERAGE_GATE_FAILED) {
                analysis->snapshot.gate_state = UMI_TEST_COVERAGE_GATE_WARNING;
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (file->gate_state == UMI_TEST_COVERAGE_GATE_PASSED) {
            ++analysis->snapshot.passed_file_count;
        }
    }
    analysis->snapshot.line_basis_points = umi_test_coverage_basis_points(
        analysis->snapshot.lines_covered, analysis->snapshot.lines_total);
    analysis->snapshot.branch_basis_points = umi_test_coverage_basis_points(
        analysis->snapshot.branches_covered, analysis->snapshot.branches_total);
    analysis->snapshot.line_delta_basis_points =
        (int32_t)analysis->snapshot.line_basis_points -
        (int32_t)baseline->line_basis_points;
    analysis->snapshot.branch_delta_basis_points =
        (int32_t)analysis->snapshot.branch_basis_points -
        (int32_t)baseline->branch_basis_points;
    analysis->snapshot.revision = ++analysis->revision;
    return UMI_STATUS_OK;
}

/*
 * Find test coverage analysis while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_test_coverage_analysis_at(
    const UmiTestCoverageAnalysis *analysis,
    size_t position,
    UmiTestCoverageFileAnalysis *out_file)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL || out_file == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= analysis->count) return UMI_STATUS_NOT_FOUND;
    *out_file = analysis->files[position];
    return UMI_STATUS_OK;
}

/*
 * Find test coverage analysis while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_test_coverage_analysis_find(
    const UmiTestCoverageAnalysis *analysis,
    const char *uri,
    UmiTestCoverageFileAnalysis *out_file)
{
    const size_t position = analysis != NULL ? find_file(analysis, uri) : (size_t)-1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL || uri == NULL || out_file == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == (size_t)-1) return UMI_STATUS_NOT_FOUND;
    *out_file = analysis->files[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the test coverage analysis snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_coverage_analysis_snapshot(
    const UmiTestCoverageAnalysis *analysis,
    UmiTestCoverageAnalysisSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (analysis == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = analysis->snapshot;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by test coverage analysis without changing
 * their state.
 */
size_t umi_test_coverage_analysis_count(const UmiTestCoverageAnalysis *analysis)
{
    return analysis != NULL ? analysis->count : 0U;
}

/*
 * Provide the test coverage gate state text operation used by this module and its client
 * applications.
 */
const char *umi_test_coverage_gate_state_text(UmiTestCoverageGateState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_TEST_COVERAGE_GATE_PASSED: return "Passed";
        case UMI_TEST_COVERAGE_GATE_WARNING: return "Warning";
        case UMI_TEST_COVERAGE_GATE_FAILED: return "Failed";
        case UMI_TEST_COVERAGE_GATE_UNKNOWN:
        default: return "Unknown";
    }
}
