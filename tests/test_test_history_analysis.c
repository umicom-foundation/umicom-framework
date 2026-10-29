/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_history_analysis.c
 *
 * PURPOSE:
 *   Verify finite completed-sample history and atomic refusal of invalid evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/history_analysis.h"

/*
 * Exercise result and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiTestPlatformResultSnapshot result(const char *id, int outcome,
                                             uint64_t sequence, double duration)
{
    UmiTestPlatformResultSnapshot value;
    (void)memset(&value, 0, sizeof(value));
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_TEST_PLATFORM_RESULT_API_VERSION;
    (void)strncpy(value.id, id, sizeof(value.id) - 1U);
    (void)strncpy(value.item_id, "case", sizeof(value.item_id) - 1U);
    value.outcome = outcome;
    value.sequence = sequence;
    value.duration_ms = duration;
    return value;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Retain the original mixed-outcome fixture for review and run it through
 * the active dispatcher alongside evidence-boundary cases. */
#if 0
int main(void)
{
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestHistoryAnalysis *analysis = NULL;
    UmiTestHistoryPolicy policy;
    UmiTestHistoryItemAnalysis item_analysis;
    UmiTestPlatformItemSnapshot item;
    UmiTestPlatformResultSnapshot a = result("a", UMI_TEST_PLATFORM_OUTCOME_PASSED, 1U, 10.0);
    UmiTestPlatformResultSnapshot b = result("b", UMI_TEST_PLATFORM_OUTCOME_FAILED, 2U, 15.0);
    UmiTestPlatformResultSnapshot c = result("c", UMI_TEST_PLATFORM_OUTCOME_PASSED, 3U, 20.0);
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_TEST_PLATFORM_ITEM_API_VERSION;
    (void)strncpy(item.id, "case", sizeof(item.id) - 1U);
    (void)strncpy(item.name, "Case", sizeof(item.name) - 1U);
    item.enabled = 1;
    assert(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    assert(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &a) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &b) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &c) == UMI_STATUS_OK);
    umi_test_history_policy_init(&policy);
    assert(umi_test_history_analysis_create(&analysis) == UMI_STATUS_OK);
    assert(umi_test_history_analysis_build(analysis, items, results, &policy) == UMI_STATUS_OK);
    assert(umi_test_history_analysis_find(analysis, "case", &item_analysis) == UMI_STATUS_OK);
    assert(item_analysis.stability == UMI_TEST_STABILITY_FLAKY);
    assert(item_analysis.sample_count == 3U && item_analysis.current_failure_streak == 0U);
    umi_test_history_analysis_destroy(analysis);
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    return 0;
}
#endif
static int Smoke(void)
{
    UmiTestPlatformItemRegistry *items = NULL;
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestHistoryAnalysis *analysis = NULL;
    UmiTestHistoryPolicy policy;
    UmiTestHistoryItemAnalysis item_analysis;
    UmiTestPlatformItemSnapshot item;
    UmiTestPlatformResultSnapshot a = result("a", UMI_TEST_PLATFORM_OUTCOME_PASSED, 1U, 10.0);
    UmiTestPlatformResultSnapshot b = result("b", UMI_TEST_PLATFORM_OUTCOME_FAILED, 2U, 15.0);
    UmiTestPlatformResultSnapshot c = result("c", UMI_TEST_PLATFORM_OUTCOME_PASSED, 3U, 20.0);
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_TEST_PLATFORM_ITEM_API_VERSION;
    (void)strncpy(item.id, "case", sizeof(item.id) - 1U);
    (void)strncpy(item.name, "Case", sizeof(item.name) - 1U);
    item.enabled = 1;
    assert(umi_test_platform_item_registry_create(&items) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    assert(umi_test_platform_item_registry_upsert(items, &item) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &a) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &b) == UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_upsert(results, &c) == UMI_STATUS_OK);
    umi_test_history_policy_init(&policy);
    assert(umi_test_history_analysis_create(&analysis) == UMI_STATUS_OK);
    assert(umi_test_history_analysis_build(analysis, items, results, &policy) == UMI_STATUS_OK);
    assert(umi_test_history_analysis_find(analysis, "case", &item_analysis) == UMI_STATUS_OK);
    assert(item_analysis.stability == UMI_TEST_STABILITY_FLAKY);
    assert(item_analysis.sample_count == 3U && item_analysis.current_failure_streak == 0U);
    umi_test_history_analysis_destroy(analysis);
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    return 0;
}

#include <float.h>
#include <math.h>

static void KeepHistory(UmiTestHistoryAnalysis *analysis,
    const UmiTestHistoryAnalysisSnapshot *before, const UmiTestHistoryItemAnalysis *item)
{
    UmiTestHistoryAnalysisSnapshot after;
    UmiTestHistoryItemAnalysis retained;
    assert(umi_test_history_analysis_snapshot(analysis,&after)==UMI_STATUS_OK);
    assert(memcmp(before,&after,sizeof(after))==0);
    assert(umi_test_history_analysis_find(analysis,"case",&retained)==UMI_STATUS_OK);
    assert(memcmp(item,&retained,sizeof(retained))==0);
    assert(umi_test_history_analysis_count(analysis)==1U);
}
int main(int argc,char **argv)
{
    if(argc==1)return Smoke();
    if(argc!=2)return 2;
    const char *name=argv[1];
    UmiTestPlatformItemRegistry *items=NULL;
    UmiTestPlatformResultRegistry *results=NULL;
    UmiTestHistoryAnalysis *analysis=NULL;
    assert(umi_test_platform_item_registry_create(&items)==UMI_STATUS_OK);
    assert(umi_test_platform_result_registry_create(&results)==UMI_STATUS_OK);
    assert(umi_test_history_analysis_create(&analysis)==UMI_STATUS_OK);
    UmiTestPlatformItemSnapshot item={0};strcpy(item.id,"case");strcpy(item.name,"Example");
    item.enabled=1;
    assert(umi_test_platform_item_registry_upsert(items,&item)==UMI_STATUS_OK);
    UmiTestPlatformResultSnapshot values[4]={
        result("a",UMI_TEST_PLATFORM_OUTCOME_PASSED,1U,10.0),
        result("b",UMI_TEST_PLATFORM_OUTCOME_PASSED,2U,10.0),
        result("c",UMI_TEST_PLATFORM_OUTCOME_PASSED,3U,10.0),
        result("d",UMI_TEST_PLATFORM_OUTCOME_PASSED,4U,10.0)};
    for(size_t i=0U;i<3U;++i)
        assert(umi_test_platform_result_registry_upsert(results,&values[i])==UMI_STATUS_OK);
    UmiTestHistoryPolicy policy;umi_test_history_policy_init(&policy);
    assert(umi_test_history_analysis_build(analysis,items,results,&policy)==UMI_STATUS_OK);
    UmiTestHistoryAnalysisSnapshot before,after;
    UmiTestHistoryItemAnalysis prior,row;
    assert(umi_test_history_analysis_snapshot(analysis,&before)==UMI_STATUS_OK);
    assert(umi_test_history_analysis_find(analysis,"case",&prior)==UMI_STATUS_OK);
    assert(prior.stability==UMI_TEST_STABILITY_STABLE);
    UmiStatus expected=UMI_STATUS_OK;
    UmiTestStabilityState stability=UMI_TEST_STABILITY_STABLE;
    size_t samples=3U;int known=1;
    if(strcmp(name,"nan-duration")==0 || strcmp(name,"infinite-duration")==0 ||
       strcmp(name,"negative-duration")==0 || strcmp(name,"unknown-outcome")==0) {
        if(strcmp(name,"nan-duration")==0)values[2].duration_ms=NAN;
        if(strcmp(name,"infinite-duration")==0)values[2].duration_ms=INFINITY;
        if(strcmp(name,"negative-duration")==0)values[2].duration_ms=-1.0;
        if(strcmp(name,"unknown-outcome")==0)values[2].outcome=99;
        expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if(strcmp(name,"policy-nan-slow")==0 || strcmp(name,"policy-infinite-slow")==0 ||
              strcmp(name,"policy-nan-trend")==0 || strcmp(name,"policy-infinite-trend")==0) {
        double invalid=strstr(name,"nan")!=NULL?NAN:INFINITY;
        if(strstr(name,"slow")!=NULL)policy.slow_duration_ms=invalid;
        else policy.duration_regression_percent=invalid;
        expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if(strcmp(name,"skipped-only")==0 || strcmp(name,"cancelled-only")==0 ||
              strcmp(name,"not-run-only")==0) {
        int outcome=strcmp(name,"skipped-only")==0?UMI_TEST_PLATFORM_OUTCOME_SKIPPED:
            strcmp(name,"cancelled-only")==0?UMI_TEST_PLATFORM_OUTCOME_CANCELLED:UMI_TEST_PLATFORM_OUTCOME_NOT_RUN;
        for(size_t i=0U;i<3U;++i)values[i].outcome=outcome;
        stability=UMI_TEST_STABILITY_UNKNOWN;
    } else if(strcmp(name,"latest-skipped")==0) {
        values[3].outcome=UMI_TEST_PLATFORM_OUTCOME_SKIPPED;samples=4U;
        stability=UMI_TEST_STABILITY_UNKNOWN;
    } else if(strcmp(name,"minimum-completed")==0) {
        values[0].outcome=UMI_TEST_PLATFORM_OUTCOME_SKIPPED;
        stability=UMI_TEST_STABILITY_UNKNOWN;
    } else if(strcmp(name,"completed-timing")==0) {
        values[0].outcome=UMI_TEST_PLATFORM_OUTCOME_SKIPPED;
        values[0].duration_ms=100000.0;samples=4U;
    } else if(strcmp(name,"maximum-duration")==0) {
        for(size_t i=0U;i<3U;++i)values[i].duration_ms=DBL_MAX;
        stability=UMI_TEST_STABILITY_SLOW;
    } else if(strcmp(name,"trend-overflow")==0) {
        values[0].duration_ms=1.0;values[1].duration_ms=DBL_MAX;values[2].duration_ms=DBL_MAX;
        expected=UMI_STATUS_CAPACITY_EXCEEDED;
    } else if(strcmp(name,"trend-drop")==0) {
        values[0].duration_ms=100.0;values[1].duration_ms=10.0;values[2].duration_ms=10.0;
    } else if(strcmp(name,"trend-rise")==0) {
        values[0].duration_ms=10.0;values[1].duration_ms=20.0;values[2].duration_ms=20.0;
        stability=UMI_TEST_STABILITY_REGRESSED;
    } else if(strcmp(name,"timed-out-streak")==0) {
        values[1].outcome=values[2].outcome=UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT;
        stability=UMI_TEST_STABILITY_FAILING;
    } else if(strcmp(name,"zero-minimum-empty")==0) {
        for(size_t i=0U;i<3U;++i)
            assert(umi_test_platform_result_registry_remove(results,values[i].id)==UMI_STATUS_OK);
        samples=0U;policy.minimum_samples=0U;stability=UMI_TEST_STABILITY_UNKNOWN;
    } else if(strcmp(name,"unrelated-invalid")==0 || strcmp(name,"late-invalid-item")==0) {
        UmiTestPlatformResultSnapshot unrelated=result("bad",99,1U,NAN);
        strcpy(unrelated.item_id,"other");
        assert(umi_test_platform_result_registry_upsert(results,&unrelated)==UMI_STATUS_OK);
        if(strcmp(name,"late-invalid-item")==0) {
            strcpy(item.id,"other");strcpy(item.name,"Other");
            assert(umi_test_platform_item_registry_upsert(items,&item)==UMI_STATUS_OK);
            expected=UMI_STATUS_INVALID_ARGUMENT;
        }
    } else if(strcmp(name,"arguments")==0) {
        assert(umi_test_history_analysis_build(NULL,items,results,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_test_history_analysis_build(analysis,NULL,results,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_test_history_analysis_build(analysis,items,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        KeepHistory(analysis,&before,&prior);
    } else known=0;
    if(known && strcmp(name,"arguments")!=0) {
        for(size_t i=0U;i<samples;++i)
            assert(umi_test_platform_result_registry_upsert(results,&values[i])==UMI_STATUS_OK);
        UmiStatus status=umi_test_history_analysis_build(analysis,items,results,&policy);
        assert(status==expected);
        if(status!=UMI_STATUS_OK)KeepHistory(analysis,&before,&prior);
        else {
            assert(umi_test_history_analysis_snapshot(analysis,&after)==UMI_STATUS_OK);
            assert(umi_test_history_analysis_find(analysis,"case",&row)==UMI_STATUS_OK);
            assert(row.sample_count==samples && row.stability==stability);
            assert(after.revision==before.revision+1U);
            assert(isfinite(row.average_duration_ms) && isfinite(row.duration_trend_percent));
            if(strcmp(name,"maximum-duration")==0)assert(row.average_duration_ms==DBL_MAX);
            if(strcmp(name,"completed-timing")==0) {
                assert(row.skipped_count==1U && row.passed_count==3U);
                assert(row.average_duration_ms==10.0 && row.minimum_duration_ms==10.0 && row.maximum_duration_ms==10.0);
                assert(row.duration_trend_percent==0.0);
            }
            if(strcmp(name,"trend-drop")==0)assert(row.duration_trend_percent==-90.0);
            if(strcmp(name,"trend-rise")==0)assert(row.duration_trend_percent==100.0);
            if(strcmp(name,"skipped-only")==0) {
                assert(row.skipped_count==3U && after.stable_count==0U);
                assert(row.average_duration_ms==0.0 && row.pass_rate_basis_points==0U);
            }
            if(strcmp(name,"timed-out-streak")==0)assert(row.timed_out_count==2U && row.current_failure_streak==2U);
        }
    }
    umi_test_history_analysis_destroy(analysis);
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_item_registry_destroy(items);
    return known?0:2;
}
