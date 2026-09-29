/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/enterprise_workspace/recovery.c
 * PURPOSE: Re-prepare frozen imports against current data without transferring stale approval.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/enterprise_workspace/recovery.h"
#include <stdlib.h>
#include <string.h>

struct UmiEnterpriseRecoveryReview {
    UmiEnterpriseRecoveryInfo info;
    char head[EWS_HEAD_CAPACITY];
    char role[UMI_ENTERPRISE_ID_CAPACITY];
    char csv[UMI_ENTERPRISE_CSV_CAPACITY];
};
static UmiStatus Check(const UmiEnterpriseWorkspace *workspace, UmiEnterpriseActor actor,
    const char *capability, const char *resource)
{
    UmiAuthorisationDecision decision;
    UmiStatus status = UmiEnterpriseWorkspaceCheckAccess(workspace, actor, capability, resource, &decision);
    return status != UMI_STATUS_OK ? status : decision.allowed ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
}
UmiStatus UmiEnterpriseRecoveryInspect(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *originalJobId,
    UmiEnterpriseRecoveryReview **outReview, UmiEnterpriseIssue *outIssue)
{
    UmiEnterpriseRecoveryReview *review;
    UmiStatus status;
    size_t index;
    if (outIssue != NULL) memset(outIssue, 0, sizeof(*outIssue));
    if (outReview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReview = NULL;
    if (workspace == NULL || !EwsId(originalJobId)) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->storageFault) return UMI_STATUS_INVALID_STATE;
    status = Check(workspace, actor, "enterprise.read", originalJobId);
    if (status != UMI_STATUS_OK) return status;
    index = EwsJobIndex(workspace->state, originalJobId);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const EwsJob *job = &workspace->state->jobs[index];
    if (job->info.state == UMI_ENTERPRISE_JOB_APPLIED) return UMI_STATUS_INVALID_STATE;
    status = Check(workspace, actor, "enterprise.job.prepare", job->info.datasetId);
    if (status != UMI_STATUS_OK) return status;
    review = calloc(1U, sizeof(*review));
    if (review == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = EwsPreview(workspace->state, job->info.datasetId, job->info.recipeId,
        job->csv, strlen(job->csv), &review->info.proposedPreview, outIssue);
    if (status == UMI_STATUS_OK) status = EwsCopy(review->head, sizeof(review->head), workspace->head);
    if (status == UMI_STATUS_OK) status = EwsCopy(review->csv, sizeof(review->csv), job->csv);
    if (status == UMI_STATUS_OK) status = EwsCopy(review->info.preparer, sizeof(review->info.preparer), actor.principal);
    if (status == UMI_STATUS_OK) status = EwsCopy(review->role, sizeof(review->role), actor.role != NULL ? actor.role : "");
    if (status != UMI_STATUS_OK) { free(review); return status; }
    review->info.originalJob = job->info;
    review->info.originalPreview = job->preview;
    review->info.targetChanged = job->info.datasetGeneration != review->info.proposedPreview.datasetGeneration;
    review->info.executionPaused = workspace->state->paused;
    *outReview = review; return UMI_STATUS_OK;
}
void UmiEnterpriseRecoveryReviewDestroy(UmiEnterpriseRecoveryReview *review) { free(review); }
UmiStatus UmiEnterpriseRecoveryDescribe(const UmiEnterpriseRecoveryReview *review,
    UmiEnterpriseRecoveryInfo *outInfo)
{
    if (review == NULL || outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outInfo = review->info; return UMI_STATUS_OK;
}
UmiStatus UmiEnterpriseRecoveryPrepare(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const UmiEnterpriseRecoveryReview *review,
    const char *newJobId, UmiEnterpriseIssue *outIssue)
{
    UmiStatus status;
    if (outIssue != NULL) memset(outIssue, 0, sizeof(*outIssue));
    if (workspace == NULL || review == NULL || !EwsId(newJobId)) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->storageFault) return UMI_STATUS_INVALID_STATE;
    status = Check(workspace, actor, "enterprise.job.prepare", review->info.originalJob.datasetId);
    if (status == UMI_STATUS_OK) status = Check(workspace, actor, "enterprise.read", review->info.originalJob.id);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(actor.principal, review->info.preparer) != 0 ||
        strcmp(actor.role != NULL ? actor.role : "", review->role) != 0) return UMI_STATUS_PERMISSION_DENIED;
    if (strcmp(workspace->head, review->head) != 0 ||
        workspace->state->revision != review->info.proposedPreview.workspaceRevision) return UMI_STATUS_BUSY;
    if (EwsJobIndex(workspace->state, newJobId) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS;
    /* Preparation reuses the canonical parser, candidate, authorisation and
     * commit path. EwsSave verifies all stored bytes in that same transaction. */
    return EwsPrepareJob(workspace, actor, newJobId, review->info.originalJob.datasetId,
        review->info.originalJob.recipeId, review->csv, strlen(review->csv),
        review->info.originalJob.id, outIssue);
}
