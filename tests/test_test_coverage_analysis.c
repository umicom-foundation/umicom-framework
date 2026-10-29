/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_coverage_analysis.c
 *
 * PURPOSE:
 *   Verify bounded coverage arithmetic, unknown evidence and atomic analysis replacement.
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
#include "umicom/test_platform/coverage_analysis.h"

/*
 * Exercise coverage and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiTestPlatformCoverageSnapshot coverage(const char *id, const char *uri,
    uint64_t total, uint64_t covered, uint64_t revision)
{
    UmiTestPlatformCoverageSnapshot value;
    (void)memset(&value, 0, sizeof(value));
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_TEST_PLATFORM_COVERAGE_API_VERSION;
    (void)strncpy(value.id, id, sizeof(value.id) - 1U);
    (void)strncpy(value.session_id, "run", sizeof(value.session_id) - 1U);
    (void)strncpy(value.uri, uri, sizeof(value.uri) - 1U);
    value.lines_total = total;
    value.lines_covered = covered;
    value.branches_total = 10U;
    value.branches_covered = covered / 10U;
    value.revision = revision;
    return value;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Preserve the original smoke entry point. It remains available through the
 * active dispatcher alongside rejection and boundary cases. */
#if 0
int main(void)
{
    UmiTestPlatformCoverageRegistry *registry = NULL;
    UmiTestCoverageAnalysis *analysis = NULL;
    UmiTestCoveragePolicy policy;
    UmiTestCoverageBaseline baseline;
    UmiTestCoverageAnalysisSnapshot snapshot;
    UmiTestCoverageFileAnalysis file;
    UmiTestPlatformCoverageSnapshot first = coverage("a", "a.c", 100U, 90U, 1U);
    UmiTestPlatformCoverageSnapshot second = coverage("b", "b.c", 100U, 50U, 2U);
    assert(umi_test_platform_coverage_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_test_platform_coverage_registry_upsert(registry, &first) == UMI_STATUS_OK);
    assert(umi_test_platform_coverage_registry_upsert(registry, &second) == UMI_STATUS_OK);
    umi_test_coverage_policy_init(&policy);
    umi_test_coverage_baseline_init(&baseline);
    baseline.line_basis_points = 6000U;
    policy.minimum_line_basis_points = 7500U;
    assert(umi_test_coverage_analysis_create(&analysis) == UMI_STATUS_OK);
    assert(umi_test_coverage_analysis_build(analysis, registry, "run", &policy,
                                             &baseline) == UMI_STATUS_OK);
    assert(umi_test_coverage_analysis_snapshot(analysis, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.file_count == 2U && snapshot.failed_file_count == 1U);
    assert(snapshot.line_basis_points == 7000U);
    assert(umi_test_coverage_analysis_find(analysis, "a.c", &file) == UMI_STATUS_OK);
    assert(file.gate_state == UMI_TEST_COVERAGE_GATE_PASSED);
    umi_test_coverage_analysis_destroy(analysis);
    umi_test_platform_coverage_registry_destroy(registry);
    return 0;
}
#endif
static int Smoke(void)
{
    UmiTestPlatformCoverageRegistry *registry = NULL;
    UmiTestCoverageAnalysis *analysis = NULL;
    UmiTestCoveragePolicy policy;
    UmiTestCoverageBaseline baseline;
    UmiTestCoverageAnalysisSnapshot snapshot;
    UmiTestCoverageFileAnalysis file;
    UmiTestPlatformCoverageSnapshot first = coverage("a", "a.c", 100U, 90U, 1U);
    UmiTestPlatformCoverageSnapshot second = coverage("b", "b.c", 100U, 50U, 2U);
    assert(umi_test_platform_coverage_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_test_platform_coverage_registry_upsert(registry, &first) == UMI_STATUS_OK);
    assert(umi_test_platform_coverage_registry_upsert(registry, &second) == UMI_STATUS_OK);
    umi_test_coverage_policy_init(&policy);
    umi_test_coverage_baseline_init(&baseline);
    baseline.line_basis_points = 6000U;
    policy.minimum_line_basis_points = 7500U;
    assert(umi_test_coverage_analysis_create(&analysis) == UMI_STATUS_OK);
    assert(umi_test_coverage_analysis_build(analysis, registry, "run", &policy,
                                             &baseline) == UMI_STATUS_OK);
    assert(umi_test_coverage_analysis_snapshot(analysis, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.file_count == 2U && snapshot.failed_file_count == 1U);
    assert(snapshot.line_basis_points == 7000U);
    assert(umi_test_coverage_analysis_find(analysis, "a.c", &file) == UMI_STATUS_OK);
    assert(file.gate_state == UMI_TEST_COVERAGE_GATE_PASSED);
    umi_test_coverage_analysis_destroy(analysis);
    umi_test_platform_coverage_registry_destroy(registry);
    return 0;
}

/* The analysis is a published snapshot. Invalid future input must not erase a
 * useful previous report or make a counter wrap look like missing coverage. */
static void Unchanged(UmiTestCoverageAnalysis *analysis,
    const UmiTestCoverageAnalysisSnapshot *before, const UmiTestCoverageFileAnalysis *file)
{
    UmiTestCoverageAnalysisSnapshot after;
    UmiTestCoverageFileAnalysis retained;
    assert(umi_test_coverage_analysis_snapshot(analysis,&after)==UMI_STATUS_OK);
    assert(memcmp(before,&after,sizeof(after))==0);
    assert(umi_test_coverage_analysis_count(analysis)==1U);
    assert(umi_test_coverage_analysis_find(analysis,"seed.c",&retained)==UMI_STATUS_OK);
    assert(memcmp(file,&retained,sizeof(retained))==0);
}
static int RatioCases(void)
{
    const struct {uint64_t covered,total;uint32_t expected;} cases[]={
        {0U,0U,0U},{1U,0U,0U},{0U,UINT64_MAX,0U},{1U,UINT64_MAX,0U},
        {UINT64_MAX,UINT64_MAX,10000U},{UINT64_MAX,1U,10000U},
        {UINT64_MAX-1U,UINT64_MAX,9999U},{UINT64_MAX/2U,UINT64_MAX,4999U},
        {UINT64_MAX/2U+1U,UINT64_MAX,5000U},{1U,3U,3333U},{2U,3U,6666U},
        {1U,10000U,1U},{1U,10001U,0U},{9999U,10000U,9999U}};
    for(size_t i=0U;i<sizeof(cases)/sizeof(cases[0]);++i)
        assert(umi_test_coverage_basis_points(cases[i].covered,cases[i].total)==cases[i].expected);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc==1)return Smoke();
    if(argc!=2)return 2;
    const char *name=argv[1];
    if(strcmp(name,"ratio-boundaries")==0)return RatioCases();
    UmiTestPlatformCoverageRegistry *registry=NULL;
    UmiTestCoverageAnalysis *analysis=NULL;
    assert(umi_test_platform_coverage_registry_create(&registry)==UMI_STATUS_OK);
    assert(umi_test_coverage_analysis_create(&analysis)==UMI_STATUS_OK);
    UmiTestPlatformCoverageSnapshot seed=coverage("seed","seed.c",100U,100U,1U);
    assert(umi_test_platform_coverage_registry_upsert(registry,&seed)==UMI_STATUS_OK);
    assert(umi_test_coverage_analysis_build(analysis,registry,"run",NULL,NULL)==UMI_STATUS_OK);
    UmiTestCoverageAnalysisSnapshot before,after;
    UmiTestCoverageFileAnalysis retained;
    assert(umi_test_coverage_analysis_snapshot(analysis,&before)==UMI_STATUS_OK);
    assert(umi_test_coverage_analysis_find(analysis,"seed.c",&retained)==UMI_STATUS_OK);
    UmiTestCoveragePolicy policy;umi_test_coverage_policy_init(&policy);
    UmiTestCoverageBaseline baseline;umi_test_coverage_baseline_init(&baseline);
    UmiStatus status=UMI_STATUS_OK;int refused=0,known=1;
    if(strcmp(name,"null-lookup")==0) {
        UmiTestCoverageFileAnalysis sentinel;memset(&sentinel,0x5a,sizeof(sentinel));
        UmiTestCoverageFileAnalysis original=sentinel;
        assert(umi_test_coverage_analysis_find(analysis,NULL,&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&original,&sentinel,sizeof(original))==0);
        assert(umi_test_coverage_analysis_find(analysis,"seed.c",NULL)==UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_test_coverage_analysis_find(NULL,"seed.c",&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
        refused=1;
    } else if(strcmp(name,"long-session")==0) {
        char session[129];memset(session,'x',128U);session[128]='\0';
        status=umi_test_coverage_analysis_build(analysis,registry,session,NULL,NULL);
        assert(status==UMI_STATUS_CAPACITY_EXCEEDED);refused=1;
    } else if(strcmp(name,"bad-policy")==0 || strcmp(name,"bad-baseline")==0) {
        if(strcmp(name,"bad-policy")==0)policy.minimum_line_basis_points=10001U;
        else baseline.line_basis_points=10001U;
        status=umi_test_coverage_analysis_build(analysis,registry,"run",&policy,&baseline);
        assert(status==UMI_STATUS_INVALID_ARGUMENT);refused=1;
    } else if(strcmp(name,"arguments")==0) {
        assert(umi_test_coverage_analysis_build(NULL,registry,"run",NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_test_coverage_analysis_build(analysis,NULL,"run",NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        refused=1;
    } else {
        assert(umi_test_platform_coverage_registry_remove(registry,"seed")==UMI_STATUS_OK);
        UmiTestPlatformCoverageSnapshot first=coverage("a","a.c",0U,0U,1U);
        UmiTestPlatformCoverageSnapshot second=coverage("b","a.c",0U,0U,2U);
        first.branches_total=first.branches_covered=0U;
        second.branches_total=second.branches_covered=0U;
        if(strncmp(name,"overflow-",9U)==0) {
            int branch=strstr(name,"branches")!=NULL;
            int aggregate=strstr(name,"aggregate")!=NULL;
            if(aggregate)strcpy(second.uri,"b.c");
            if(branch){first.branches_total=first.branches_covered=UINT64_MAX;second.branches_total=second.branches_covered=1U;}
            else {first.lines_total=first.lines_covered=UINT64_MAX;second.lines_total=second.lines_covered=1U;}
            assert(umi_test_platform_coverage_registry_upsert(registry,&first)==UMI_STATUS_OK);
            assert(umi_test_platform_coverage_registry_upsert(registry,&second)==UMI_STATUS_OK);
            status=umi_test_coverage_analysis_build(analysis,registry,"run",NULL,NULL);
            assert(status==UMI_STATUS_CAPACITY_EXCEEDED);refused=1;
        } else if(strcmp(name,"invalid-lines")==0 || strcmp(name,"invalid-branches")==0) {
            if(strcmp(name,"invalid-lines")==0)first.lines_covered=1U;
            else first.branches_covered=1U;
            assert(umi_test_platform_coverage_registry_upsert(registry,&first)==UMI_STATUS_OK);
            status=umi_test_coverage_analysis_build(analysis,registry,"run",NULL,NULL);
            assert(status==UMI_STATUS_INVALID_ARGUMENT);refused=1;
        } else if(strcmp(name,"maximum-sum")==0 || strcmp(name,"large-ratio")==0) {
            first.lines_total=UINT64_MAX-1U;
            first.lines_covered=strcmp(name,"large-ratio")==0?(UINT64_MAX/2U):UINT64_MAX-1U;
            second.lines_total=second.lines_covered=1U;
            first.branches_total=first.branches_covered=UINT64_MAX-1U;
            second.branches_total=second.branches_covered=1U;
            assert(umi_test_platform_coverage_registry_upsert(registry,&first)==UMI_STATUS_OK);
            assert(umi_test_platform_coverage_registry_upsert(registry,&second)==UMI_STATUS_OK);
            assert(umi_test_coverage_analysis_build(analysis,registry,"run",NULL,NULL)==UMI_STATUS_OK);
            assert(umi_test_coverage_analysis_snapshot(analysis,&after)==UMI_STATUS_OK);
            assert(after.lines_total==UINT64_MAX && after.branches_total==UINT64_MAX);
            assert(after.line_basis_points==(strcmp(name,"large-ratio")==0?5000U:10000U));
            assert(after.branch_basis_points==10000U && after.revision==before.revision+1U);
        } else if(strcmp(name,"empty-unknown")==0 || strcmp(name,"empty-allowed")==0 ||
                  strcmp(name,"mixed-unknown")==0 || strcmp(name,"unknown-warning")==0 ||
                  strcmp(name,"unknown-failed")==0) {
            int mixed=strcmp(name,"mixed-unknown")==0 || strncmp(name,"unknown-",8U)==0;
            policy.allow_empty=strcmp(name,"empty-allowed")==0;
            assert(umi_test_platform_coverage_registry_upsert(registry,&first)==UMI_STATUS_OK);
            if(mixed) {
                strcpy(second.uri,"b.c");second.lines_total=second.lines_covered=100U;
                second.branches_total=second.branches_covered=10U;
                if(strcmp(name,"unknown-warning")==0)second.branches_covered=1U;
                if(strcmp(name,"unknown-failed")==0)second.lines_covered=1U;
                assert(umi_test_platform_coverage_registry_upsert(registry,&second)==UMI_STATUS_OK);
            }
            assert(umi_test_coverage_analysis_build(analysis,registry,"run",&policy,NULL)==UMI_STATUS_OK);
            assert(umi_test_coverage_analysis_snapshot(analysis,&after)==UMI_STATUS_OK);
            UmiTestCoverageGateState expected=policy.allow_empty?UMI_TEST_COVERAGE_GATE_PASSED:UMI_TEST_COVERAGE_GATE_UNKNOWN;
            if(strcmp(name,"unknown-warning")==0)expected=UMI_TEST_COVERAGE_GATE_WARNING;
            if(strcmp(name,"unknown-failed")==0)expected=UMI_TEST_COVERAGE_GATE_FAILED;
            assert(after.gate_state==expected);
        } else if(strcmp(name,"session-filter")==0) {
            strcpy(first.session_id,"other");first.lines_covered=1U;
            assert(umi_test_platform_coverage_registry_upsert(registry,&first)==UMI_STATUS_OK);
            assert(umi_test_coverage_analysis_build(analysis,registry,"run",NULL,NULL)==UMI_STATUS_OK);
            assert(umi_test_coverage_analysis_snapshot(analysis,&after)==UMI_STATUS_OK);
            assert(after.file_count==0U && after.gate_state==UMI_TEST_COVERAGE_GATE_UNKNOWN);
        } else known=0;
    }
    if(refused)Unchanged(analysis,&before,&retained);
    umi_test_coverage_analysis_destroy(analysis);
    umi_test_platform_coverage_registry_destroy(registry);
    return known?0:2;
}
