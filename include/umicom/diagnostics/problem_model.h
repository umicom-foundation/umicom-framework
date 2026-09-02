/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/problem_model.h
 *
 * PURPOSE:
 *   Define the toolkit-neutral Problems model produced after provider findings
 *   pass through severity policy, suppression and baseline classification.
 *   GTK4, Qt, Wt, headless automation and future applications query the same
 *   model and therefore show identical problem state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_PROBLEM_MODEL_H
#define UMICOM_DIAGNOSTICS_PROBLEM_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/diagnostics/baseline.h"
#include "umicom/diagnostics/severity_policy.h"
#include "umicom/diagnostics/suppression.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_PROBLEM_MODEL_API_VERSION 1U
#define UMI_DIAGNOSTIC_PROBLEM_QUERY_RESULT_MAX 64U
#define UMI_DIAGNOSTIC_PROBLEM_TEXT_CAPACITY 256U

/**
 * List the named diagnostic problem sort values accepted by this public contract.
 */
typedef enum UmiDiagnosticProblemSort {
    UMI_DIAGNOSTIC_PROBLEM_SORT_SEVERITY = 1,
    UMI_DIAGNOSTIC_PROBLEM_SORT_LOCATION = 2,
    UMI_DIAGNOSTIC_PROBLEM_SORT_PROVIDER = 3,
    UMI_DIAGNOSTIC_PROBLEM_SORT_BASELINE = 4,
    UMI_DIAGNOSTIC_PROBLEM_SORT_NEWEST = 5
} UmiDiagnosticProblemSort;

/**
 * List the named diagnostic problem group values accepted by this public contract.
 */
typedef enum UmiDiagnosticProblemGroup {
    UMI_DIAGNOSTIC_PROBLEM_GROUP_NONE = 0,
    UMI_DIAGNOSTIC_PROBLEM_GROUP_FILE = 1,
    UMI_DIAGNOSTIC_PROBLEM_GROUP_PROVIDER = 2,
    UMI_DIAGNOSTIC_PROBLEM_GROUP_SOURCE = 3,
    UMI_DIAGNOSTIC_PROBLEM_GROUP_SEVERITY = 4,
    UMI_DIAGNOSTIC_PROBLEM_GROUP_CODE = 5
} UmiDiagnosticProblemGroup;

/**
 * Represent the diagnostic problem data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticProblem {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDiagnosticProviderFinding finding;
    UmiDiagnosticSeverity original_severity;
    UmiDiagnosticSeverity effective_severity;
    UmiDiagnosticBaselineState baseline_state;
    UmiDiagnosticSuppressionDecision suppression;
    char severity_rule_id[UMI_DIAGNOSTIC_POLICY_ID_CAPACITY];
    uint64_t first_seen_run_id;
    uint64_t last_seen_run_id;
    uint64_t revision;
    int active;
    int selected;
    int policy_disabled;
    int fixable;
} UmiDiagnosticProblem;

/**
 * Represent the diagnostic problem model snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDiagnosticProblemModelSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t problem_count;
    size_t active_count;
    size_t resolved_count;
    size_t suppressed_count;
    size_t disabled_count;
    size_t selected_count;
    size_t fixable_count;
    size_t new_count;
    size_t updated_count;
    size_t by_effective_severity[UMI_DIAGNOSTIC_SEVERITY_COUNT];
    uint64_t revision;
} UmiDiagnosticProblemModelSnapshot;

/**
 * Represent the diagnostic problem query data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticProblemQuery {
    uint32_t severity_mask;
    uint32_t kind_mask;
    uint32_t baseline_mask;
    char provider[UMI_DIAGNOSTIC_PROVIDER_ID_CAPACITY];
    char source[UMI_DIAGNOSTIC_SOURCE_CAPACITY];
    char code[UMI_DIAGNOSTIC_CODE_CAPACITY];
    char uri[UMI_DIAGNOSTIC_URI_CAPACITY];
    char text[UMI_DIAGNOSTIC_PROBLEM_TEXT_CAPACITY];
    UmiDiagnosticProblemSort sort;
    UmiDiagnosticProblemGroup group;
    size_t offset;
    size_t limit;
    int include_suppressed;
    int include_resolved;
    int include_disabled;
    int only_fixable;
    int only_selected;
} UmiDiagnosticProblemQuery;

/**
 * Represent the diagnostic problem query result data shared with callers of this public
 * contract.
 */
typedef struct UmiDiagnosticProblemQueryResult {
    UmiDiagnosticProblem items[UMI_DIAGNOSTIC_PROBLEM_QUERY_RESULT_MAX];
    size_t count;
    size_t total_matches;
    size_t group_count;
    int truncated;
} UmiDiagnosticProblemQueryResult;

/**
 * Represent the diagnostic problem model data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticProblemModel UmiDiagnosticProblemModel;

/**
 * Initialise diagnostic problem model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_problem_model_create(
    UmiDiagnosticProblemModel **out_model);
/**
 * Release or reset state held by diagnostic problem model so the same storage can be
 * reused safely.
 */
void umi_diagnostic_problem_model_destroy(UmiDiagnosticProblemModel *model);
/**
 * Release or reset state held by diagnostic problem model so the same storage can be
 * reused safely.
 */
UmiStatus umi_diagnostic_problem_model_clear(UmiDiagnosticProblemModel *model);
/**
 * Perform diagnostic problem model begin provider through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_diagnostic_problem_model_begin_provider_run(
    UmiDiagnosticProblemModel *model,
    const char *provider_id,
    uint64_t run_id);
/**
 * Provide the diagnostic problem model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_problem_model_upsert(
    UmiDiagnosticProblemModel *model,
    const UmiDiagnosticProblem *problem);
/**
 * Perform diagnostic problem model finish provider through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_diagnostic_problem_model_finish_provider_run(
    UmiDiagnosticProblemModel *model,
    const char *provider_id,
    uint64_t run_id,
    size_t *out_resolved_count);
/**
 * Remove diagnostic problem model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_diagnostic_problem_model_remove(
    UmiDiagnosticProblemModel *model,
    uint64_t fingerprint);
/**
 * Find diagnostic problem model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_diagnostic_problem_model_find(
    const UmiDiagnosticProblemModel *model,
    uint64_t fingerprint,
    UmiDiagnosticProblem *out_problem);
/**
 * Find diagnostic problem model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_diagnostic_problem_model_at(
    const UmiDiagnosticProblemModel *model,
    size_t position,
    UmiDiagnosticProblem *out_problem);
/**
 * Provide the diagnostic problem model select operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_problem_model_select(
    UmiDiagnosticProblemModel *model,
    uint64_t fingerprint,
    int selected);
/**
 * Provide the diagnostic problem model clear selection operation used by this module and
 * its client applications.
 */
UmiStatus umi_diagnostic_problem_model_clear_selection(
    UmiDiagnosticProblemModel *model);
/**
 * Initialise diagnostic problem query from caller-provided values so later operations
 * receive a known state.
 */
void umi_diagnostic_problem_query_init(UmiDiagnosticProblemQuery *query);
/**
 * Perform diagnostic problem query through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_diagnostic_problem_query_execute(
    const UmiDiagnosticProblemModel *model,
    const UmiDiagnosticProblemQuery *query,
    UmiDiagnosticProblemQueryResult *out_result);
/**
 * Provide the diagnostic problem model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_problem_model_snapshot(
    const UmiDiagnosticProblemModel *model,
    UmiDiagnosticProblemModelSnapshot *out_snapshot);
/**
 * Return the number of records represented by diagnostic problem model without changing
 * their state.
 */
size_t umi_diagnostic_problem_model_count(const UmiDiagnosticProblemModel *model);
/**
 * Provide the diagnostic problem model revision operation used by this module and its
 * client applications.
 */
uint64_t umi_diagnostic_problem_model_revision(
    const UmiDiagnosticProblemModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DIAGNOSTICS_PROBLEM_MODEL_H */
