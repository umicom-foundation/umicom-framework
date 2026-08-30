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

typedef enum UmiDiagnosticProblemSort {
    UMI_DIAGNOSTIC_PROBLEM_SORT_SEVERITY = 1,
    UMI_DIAGNOSTIC_PROBLEM_SORT_LOCATION = 2,
    UMI_DIAGNOSTIC_PROBLEM_SORT_PROVIDER = 3,
    UMI_DIAGNOSTIC_PROBLEM_SORT_BASELINE = 4,
    UMI_DIAGNOSTIC_PROBLEM_SORT_NEWEST = 5
} UmiDiagnosticProblemSort;

typedef enum UmiDiagnosticProblemGroup {
    UMI_DIAGNOSTIC_PROBLEM_GROUP_NONE = 0,
    UMI_DIAGNOSTIC_PROBLEM_GROUP_FILE = 1,
    UMI_DIAGNOSTIC_PROBLEM_GROUP_PROVIDER = 2,
    UMI_DIAGNOSTIC_PROBLEM_GROUP_SOURCE = 3,
    UMI_DIAGNOSTIC_PROBLEM_GROUP_SEVERITY = 4,
    UMI_DIAGNOSTIC_PROBLEM_GROUP_CODE = 5
} UmiDiagnosticProblemGroup;

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

typedef struct UmiDiagnosticProblemQueryResult {
    UmiDiagnosticProblem items[UMI_DIAGNOSTIC_PROBLEM_QUERY_RESULT_MAX];
    size_t count;
    size_t total_matches;
    size_t group_count;
    int truncated;
} UmiDiagnosticProblemQueryResult;

typedef struct UmiDiagnosticProblemModel UmiDiagnosticProblemModel;

UmiStatus umi_diagnostic_problem_model_create(
    UmiDiagnosticProblemModel **out_model);
void umi_diagnostic_problem_model_destroy(UmiDiagnosticProblemModel *model);
UmiStatus umi_diagnostic_problem_model_clear(UmiDiagnosticProblemModel *model);
UmiStatus umi_diagnostic_problem_model_begin_provider_run(
    UmiDiagnosticProblemModel *model,
    const char *provider_id,
    uint64_t run_id);
UmiStatus umi_diagnostic_problem_model_upsert(
    UmiDiagnosticProblemModel *model,
    const UmiDiagnosticProblem *problem);
UmiStatus umi_diagnostic_problem_model_finish_provider_run(
    UmiDiagnosticProblemModel *model,
    const char *provider_id,
    uint64_t run_id,
    size_t *out_resolved_count);
UmiStatus umi_diagnostic_problem_model_remove(
    UmiDiagnosticProblemModel *model,
    uint64_t fingerprint);
UmiStatus umi_diagnostic_problem_model_find(
    const UmiDiagnosticProblemModel *model,
    uint64_t fingerprint,
    UmiDiagnosticProblem *out_problem);
UmiStatus umi_diagnostic_problem_model_at(
    const UmiDiagnosticProblemModel *model,
    size_t position,
    UmiDiagnosticProblem *out_problem);
UmiStatus umi_diagnostic_problem_model_select(
    UmiDiagnosticProblemModel *model,
    uint64_t fingerprint,
    int selected);
UmiStatus umi_diagnostic_problem_model_clear_selection(
    UmiDiagnosticProblemModel *model);
void umi_diagnostic_problem_query_init(UmiDiagnosticProblemQuery *query);
UmiStatus umi_diagnostic_problem_query_execute(
    const UmiDiagnosticProblemModel *model,
    const UmiDiagnosticProblemQuery *query,
    UmiDiagnosticProblemQueryResult *out_result);
UmiStatus umi_diagnostic_problem_model_snapshot(
    const UmiDiagnosticProblemModel *model,
    UmiDiagnosticProblemModelSnapshot *out_snapshot);
size_t umi_diagnostic_problem_model_count(const UmiDiagnosticProblemModel *model);
uint64_t umi_diagnostic_problem_model_revision(
    const UmiDiagnosticProblemModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DIAGNOSTICS_PROBLEM_MODEL_H */
