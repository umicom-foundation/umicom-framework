/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_workflow.c
 * Purpose: Exercise real orchestration state with inert host callbacks and exact synthetic reports.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/distribution/runtime/qualification_workflow.h"
#include "umicom/distribution/runtime/qualification_view.h"
#include <stdlib.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "Callback check at %d: %s\n", __LINE__, #expr); exit(1); } } while (0)
typedef struct Probe {
    const char *name;
    unsigned builds, tests, tools, reads, progress;
    bool cancel;
    int lastStage;
    char execution[65], toolchain[65];
} Probe;
static bool Is(Probe *p, const char *name) { return strcmp(p->name, name) == 0; }
static bool Cancel(void *context) { return ((Probe *)context)->cancel; }
static UmiStatus Read(void *context, const char *path, size_t limit, size_t *size, char digest[65])
{
    Probe *p = context; ++p->reads;
    if ((Is(p, "source-before-fails") && p->reads == 1U) || (Is(p, "source-after-fails") && p->reads == 2U))
        return UMI_STATUS_IO_ERROR;
    if (Is(p, "cancel-after-observation") && p->reads == 2U) p->cancel = true;
    return QualificationRead(Is(p, "source-changed") && p->reads == 2U ? "changed" : NULL, path, limit, size, digest);
}
static UmiStatus Toolchain(char output[65], void *context)
{
    Probe *p = context; ++p->tools;
    if ((Is(p, "tool-before-fails") && p->tools == 1U) || (Is(p, "tool-after-fails") && p->tools == 2U))
        return UMI_STATUS_IO_ERROR;
    memcpy(output, p->toolchain, 65U);
    if ((Is(p, "tool-before-invalid") && p->tools == 1U) || (Is(p, "tool-after-invalid") && p->tools == 2U))
        memset(output, 'x', 65U);
    if (Is(p, "tool-changed") && p->tools == 2U)
        CHECK(UmiSha256Buffer("different", 9U, output) == UMI_STATUS_OK);
    return UMI_STATUS_OK;
}
static UmiStatus Build(const UmiReleaseImpactPlan *plan, const char *execution,
    UmiReleaseQualificationResult *out, void *context)
{
    Probe *p = context; ++p->builds;
    CHECK(UmiReleaseImpactCount(plan) != 0U && strcmp(execution, p->execution) == 0);
    *out = QualificationResult(Is(p, "build-failed") ? UMI_QUALIFICATION_FAILED : UMI_QUALIFICATION_PASSED);
    if (Is(p, "build-invalid")) out->exitCodeKnown = false;
    return Is(p, "build-callback-fails") ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static UmiStatus Tests(const UmiReleaseImpactPlan *plan, const char *execution,
    const char **xml, size_t *length, UmiReleaseQualificationResult *process, void *context)
{
    Probe *p = context; ++p->tests;
    CHECK(UmiReleaseImpactCount(plan) != 0U && strcmp(execution, p->execution) == 0);
    *xml = "<testsuite tests='2'><testcase name='test.000000' status='run' time='0'/><testcase name='test.000001' status='run' time='0'/></testsuite>";
    *process = QualificationResult(UMI_QUALIFICATION_PASSED);
    if (Is(p, "tests-failed")) {
        *xml = "<testsuite tests='2'><testcase name='test.000000' status='fail' time='0'><failure message='Failed'/></testcase><testcase name='test.000001' status='run' time='0'/></testsuite>";
        *process = QualificationResult(UMI_QUALIFICATION_FAILED);
    }
    if (Is(p, "runner-failed")) *process = QualificationResult(UMI_QUALIFICATION_FAILED);
    if (Is(p, "missing-test")) *xml = "<testsuite tests='1'><testcase name='test.000000' status='run' time='0'/></testsuite>";
    if (Is(p, "wrong-test")) *xml = "<testsuite tests='2'><testcase name='wrong' status='run' time='0'/><testcase name='test.000001' status='run' time='0'/></testsuite>";
    if (Is(p, "malformed")) *xml = "not XML";
    *length = strlen(*xml);
    return Is(p, "tests-callback-fails") ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static void Progress(UmiQualificationWorkflowStage stage, const UmiReleaseQualification *run, void *context)
{
    Probe *p = context;
    CHECK((int)stage > p->lastStage); p->lastStage = (int)stage; ++p->progress;
    UmiQualificationView *view = NULL;
    CHECK(UmiQualificationViewCapture(run, &view) == UMI_STATUS_OK);
    UmiQualificationViewSummary summary;
    CHECK(UmiQualificationViewDescribe(view, &summary) == UMI_STATUS_OK);
    CHECK(summary.results.closed == (stage == UMI_QUALIFICATION_WORKFLOW_CLOSED));
    UmiQualificationViewDestroy(view);
    if ((Is(p, "cancel-prepared") && stage == UMI_QUALIFICATION_WORKFLOW_PREPARED) ||
        (Is(p, "cancel-after-build") && stage == UMI_QUALIFICATION_WORKFLOW_BUILD_RECORDED) ||
        (Is(p, "cancel-after-tests") && stage == UMI_QUALIFICATION_WORKFLOW_TESTS_RECORDED)) p->cancel = true;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    Probe p = {.name = argv[1], .lastStage = -1};
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, Is(&p, "no-tests") ? 0U : 2U, false) == 0);
    memcpy(p.execution, f.execution, 65U); memcpy(p.toolchain, f.toolchain, 65U);
    UmiReleaseSourceOptions options = {.maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT,
        .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT, .read = Read, .context = &p};
    UmiQualificationWorkflowRequest request = {.inventory = f.inventory, .plan = f.plan,
        .execution = p.execution, .sourceOptions = &options,
        .host = {.build = Build, .tests = Tests, .toolchain = Toolchain, .cancelled = Cancel, .progress = Progress, .context = &p}};
    if (Is(&p, "cancel-before")) p.cancel = true;
    if (Is(&p, "arguments")) request.host.build = NULL;
    UmiReleaseQualification *run = NULL;
    UmiStatus status = UmiQualificationWorkflowExecute(&request, &run);
    bool noRun = Is(&p, "cancel-before") || Is(&p, "arguments") || Is(&p, "tool-before-fails") ||
        Is(&p, "tool-before-invalid") || Is(&p, "source-before-fails");
    if (noRun) {
        REQUIRE(status != UMI_STATUS_OK && run == NULL && p.builds == 0U && p.tests == 0U && p.progress == 0U);
    } else {
        REQUIRE(run != NULL && p.lastStage == UMI_QUALIFICATION_WORKFLOW_CLOSED);
        UmiReleaseQualificationSummary summary;
        REQUIRE(UmiReleaseQualificationDescribe(run, &summary) == UMI_STATUS_OK && summary.closed);
        REQUIRE(summary.readyForOwnerReview == Is(&p, "passed"));
        REQUIRE((status == UMI_STATUS_OK) == Is(&p, "passed"));
        if (Is(&p, "build-failed") || Is(&p, "build-callback-fails") || Is(&p, "build-invalid") ||
            Is(&p, "cancel-prepared") || Is(&p, "cancel-after-build") || Is(&p, "no-tests")) REQUIRE(p.tests == 0U);
        if (Is(&p, "cancel-prepared")) REQUIRE(p.builds == 0U);
        if (Is(&p, "cancel-after-tests") || Is(&p, "cancel-after-observation")) {
            REQUIRE(summary.passed == 2U && summary.executionStatus == UMI_STATUS_CANCELLED);
            REQUIRE((summary.blockers & UMI_QUALIFICATION_EXECUTION_INTERRUPTED) != 0U);
            REQUIRE(status == UMI_STATUS_CANCELLED);
        }
        if (Is(&p, "source-changed")) REQUIRE((summary.blockers & UMI_QUALIFICATION_SOURCE_CHANGED) != 0U);
        if (Is(&p, "tool-changed")) REQUIRE((summary.blockers & UMI_QUALIFICATION_TOOLCHAIN_CHANGED) != 0U);
        if (Is(&p, "tool-after-invalid")) REQUIRE((summary.blockers & UMI_QUALIFICATION_TOOLCHAIN_UNOBSERVED) != 0U);
        if (Is(&p, "runner-failed")) REQUIRE(summary.passed == 2U && (summary.blockers & UMI_QUALIFICATION_TEST_PROCESS_NOT_PASSED) != 0U);
        if (Is(&p, "malformed") || Is(&p, "missing-test") || Is(&p, "wrong-test") || Is(&p, "tests-callback-fails"))
            REQUIRE(summary.recorded == 0U && !summary.testProcessRecorded);
    }
    UmiReleaseQualificationDestroy(run); QualificationDestroy(&f);
    return 0;
}
