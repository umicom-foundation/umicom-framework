/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_workflow.c
 * Purpose: Sequence host execution and evidence capture without treating missing results as success.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/distribution/runtime/qualification_workflow.h"
#include "umicom/distribution/runtime/qualification_ctest.h"
#include "umicom/build/directory_guard.h"
#include <string.h>
static bool Cancelled(const UmiQualificationWorkflowHost *host)
{
    return host->cancelled != NULL && host->cancelled(host->context);
}
static void Progress(const UmiQualificationWorkflowHost *host, UmiQualificationWorkflowStage stage,
    const UmiReleaseQualification *run)
{
    if (host->progress != NULL) host->progress(stage, run, host->context);
}
UmiStatus UmiQualificationWorkflowExecute(const UmiQualificationWorkflowRequest *request,
    UmiReleaseQualification **outRun)
{
    if (request == NULL || outRun == NULL || *outRun != NULL ||
        request->inventory == NULL || request->plan == NULL || request->execution == NULL ||
        request->host.build == NULL || request->host.tests == NULL || request->host.toolchain == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiQualificationWorkflowHost *host = &request->host;
    if (Cancelled(host)) return UMI_STATUS_CANCELLED;
    UmiReleaseImpactSummary selection;
    UmiStatus status = UmiReleaseImpactDescribe(request->plan, &selection);
    if (status != UMI_STATUS_OK) return status;
    if (selection.noWork) return UMI_STATUS_UNAVAILABLE;
    UmiFileLease *lease = NULL;
    if (request->guardBuildDirectory) {
        status = UmiBuildDirectoryGuardAcquire(UmiReleaseInventoryBuildRoot(request->inventory), &lease);
        if (status != UMI_STATUS_OK) return status;
    }
    char toolchain[65] = "", afterToolchain[65] = "";
    UmiReleaseSourceObservation *before = NULL, *after = NULL;
    UmiReleaseQualification *run = NULL;
    UmiReleaseQualificationIdentity identity;
    status = host->toolchain(toolchain, host->context);
    if (status == UMI_STATUS_OK && Cancelled(host)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = UmiReleaseSourceObserve(request->inventory, request->sourceOptions, &before);
    if (status == UMI_STATUS_OK && Cancelled(host)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = UmiReleaseQualificationCreate(request->plan, before, request->execution, toolchain, &run);
    if (status == UMI_STATUS_OK) status = UmiReleaseQualificationGetIdentity(run, &identity);
    if (status == UMI_STATUS_OK) {
        Progress(host, UMI_QUALIFICATION_WORKFLOW_PREPARED, run);
        if (Cancelled(host)) status = UMI_STATUS_CANCELLED;
    }
    UmiReleaseQualificationResult build = {0};
    if (status == UMI_STATUS_OK) status = host->build(request->plan, request->execution, &build, host->context);
    if (status == UMI_STATUS_OK) {
        status = UmiReleaseQualificationRecordBuild(run, &identity, &build);
        if (status == UMI_STATUS_OK) {
            Progress(host, UMI_QUALIFICATION_WORKFLOW_BUILD_RECORDED, run);
            if (build.outcome != UMI_QUALIFICATION_PASSED) status = UMI_STATUS_INVALID_STATE;
        }
    }
    if (status == UMI_STATUS_OK && Cancelled(host)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK && selection.tests != 0U) {
        const char *xml = NULL; size_t length = 0U;
        UmiReleaseQualificationResult process = {0};
        status = host->tests(request->plan, request->execution, &xml, &length, &process, host->context);
        if (status == UMI_STATUS_OK)
            status = UmiReleaseQualificationImportCtest(run, &identity, xml, length, &process);
        if (status == UMI_STATUS_OK) Progress(host, UMI_QUALIFICATION_WORKFLOW_TESTS_RECORDED, run);
    }
    if (status == UMI_STATUS_OK && Cancelled(host)) status = UMI_STATUS_CANCELLED;

    UmiStatus sourceStatus = UMI_STATUS_UNAVAILABLE, toolStatus = UMI_STATUS_UNAVAILABLE;
    if (run != NULL) {
        /* Final observations remain useful even after failure. They are evidence,
         * not permission to overwrite the first execution failure. */
        sourceStatus = UmiReleaseSourceObserve(request->inventory, request->sourceOptions, &after);
        if (status == UMI_STATUS_OK && sourceStatus != UMI_STATUS_OK) status = sourceStatus;
        toolStatus = host->toolchain(afterToolchain, host->context);
        if (status == UMI_STATUS_OK && toolStatus != UMI_STATUS_OK) status = toolStatus;
        if (status == UMI_STATUS_OK && Cancelled(host)) status = UMI_STATUS_CANCELLED;
    }
    /* No executor or source probe runs after this release. Account for release
     * failures before sealing so the retained ledger cannot say ready when
     * orchestration itself failed after its last passing result. */
    UmiStatus releaseStatus = UmiFileLeaseRelease(&lease);
    if (status == UMI_STATUS_OK && releaseStatus != UMI_STATUS_OK) status = releaseStatus;
    if (run != NULL) {
        if (status != UMI_STATUS_OK)
            (void)UmiReleaseQualificationInterrupt(run, &identity, status);
        UmiStatus closeStatus = UmiReleaseQualificationClose(run, &identity, after,
            toolStatus == UMI_STATUS_OK ? afterToolchain : NULL);
        if (closeStatus != UMI_STATUS_OK) {
            if (status == UMI_STATUS_OK) {
                status = closeStatus;
                (void)UmiReleaseQualificationInterrupt(run, &identity, status);
            }
            /* An invalid returned digest cannot leave a mutable attempt behind.
             * Seal it as unobserved while retaining the callback's first error. */
            closeStatus = UmiReleaseQualificationClose(run, &identity, after, NULL);
        }
        if (status == UMI_STATUS_OK && closeStatus != UMI_STATUS_OK) status = closeStatus;
        UmiReleaseQualificationSummary summary;
        UmiStatus describeStatus = UmiReleaseQualificationDescribe(run, &summary);
        if (status == UMI_STATUS_OK && describeStatus != UMI_STATUS_OK) status = describeStatus;
        if (status == UMI_STATUS_OK && !summary.readyForOwnerReview) status = UMI_STATUS_INVALID_STATE;
        Progress(host, UMI_QUALIFICATION_WORKFLOW_CLOSED, run);
        *outRun = run;
    }
    UmiReleaseSourceObservationDestroy(after); UmiReleaseSourceObservationDestroy(before);
    return status;
}
