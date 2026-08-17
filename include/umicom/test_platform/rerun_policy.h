/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/rerun_policy.h
 *
 * PURPOSE:
 *   Derive auditable rerun selections from retained outcomes, flakiness,
 *   duration and failure streaks rather than leaving rerun logic in an IDE.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_RERUN_POLICY_H
#define UMICOM_TEST_PLATFORM_RERUN_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/test_platform/history.h"
#include "umicom/test_platform/operation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_RERUN_POLICY_API_VERSION 1U

typedef enum UmiTestRerunReason {
    UMI_TEST_RERUN_REASON_NONE = 0U,
    UMI_TEST_RERUN_REASON_FAILED = 1U << 0,
    UMI_TEST_RERUN_REASON_TIMED_OUT = 1U << 1,
    UMI_TEST_RERUN_REASON_FLAKY = 1U << 2,
    UMI_TEST_RERUN_REASON_SLOW = 1U << 3,
    UMI_TEST_RERUN_REASON_NOT_RUN = 1U << 4,
    UMI_TEST_RERUN_REASON_SKIPPED = 1U << 5
} UmiTestRerunReason;

typedef struct UmiTestRerunPolicy {
    uint32_t struct_size;
    uint32_t api_version;
    uint32_t reason_mask;
    size_t maximum_candidates;
    size_t minimum_failure_streak;
    double slow_duration_ms;
    uint32_t repeat_count;
    int stop_on_failure;
    int include_disabled;
} UmiTestRerunPolicy;

typedef struct UmiTestRerunCandidate {
    uint32_t struct_size;
    uint32_t api_version;
    char item_id[128];
    char item_name[256];
    UmiTestPlatformResultSnapshot latest_result;
    uint32_t reason_mask;
    size_t sample_count;
    size_t failure_count;
    size_t failure_streak;
    double average_duration_ms;
    int32_t score;
    int selected;
} UmiTestRerunCandidate;

typedef struct UmiTestRerunPlanSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t candidate_count;
    size_t selected_count;
    size_t failed_count;
    size_t timed_out_count;
    size_t flaky_count;
    size_t slow_count;
    size_t not_run_count;
    uint64_t item_revision;
    uint64_t result_revision;
    uint64_t revision;
} UmiTestRerunPlanSnapshot;

typedef struct UmiTestRerunPlan UmiTestRerunPlan;

void umi_test_rerun_policy_init(UmiTestRerunPolicy *policy);
UmiStatus umi_test_rerun_plan_create(UmiTestRerunPlan **out_plan);
void umi_test_rerun_plan_destroy(UmiTestRerunPlan *plan);
UmiStatus umi_test_rerun_plan_build(
    UmiTestRerunPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestRerunPolicy *policy);
UmiStatus umi_test_rerun_plan_set_selected(
    UmiTestRerunPlan *plan,
    const char *item_id,
    int selected);
UmiStatus umi_test_rerun_plan_select_all(UmiTestRerunPlan *plan, int selected);
UmiStatus umi_test_rerun_plan_selection(
    const UmiTestRerunPlan *plan,
    UmiTestPlatformSelection *out_selection);
UmiStatus umi_test_rerun_plan_operation(
    const UmiTestRerunPlan *plan,
    const UmiTestRerunPolicy *policy,
    UmiTestPlatformOperationPlan *out_operation);
UmiStatus umi_test_rerun_plan_at(
    const UmiTestRerunPlan *plan,
    size_t position,
    UmiTestRerunCandidate *out_candidate);
UmiStatus umi_test_rerun_plan_snapshot(
    const UmiTestRerunPlan *plan,
    UmiTestRerunPlanSnapshot *out_snapshot);
size_t umi_test_rerun_plan_count(const UmiTestRerunPlan *plan);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_TEST_PLATFORM_RERUN_POLICY_H */
