/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/enterprise_workspace/workspace.c
 *
 * PURPOSE:
 *   Coordinate immutable import plans, capability checks and atomic Data Server commits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>
#include <string.h>

static bool ActorValid(UmiEnterpriseActor actor)
{
    return EwsId(actor.principal) && (actor.role == NULL || actor.role[0] == '\0' || EwsId(actor.role));
}
UmiStatus UmiEnterpriseWorkspaceCheckAccess(const UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *capability, const char *resource,
    UmiAuthorisationDecision *outDecision)
{
    if (workspace == NULL || !ActorValid(actor) || capability == NULL || capability[0] == '\0' ||
        resource == NULL || resource[0] == '\0' || outDecision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_authorisation_check(workspace->authorisation, actor.principal, actor.role,
        capability, resource, outDecision);
}
static UmiStatus Permit(UmiEnterpriseWorkspace *workspace, UmiEnterpriseActor actor,
    const char *capability, const char *resource)
{
    UmiAuthorisationDecision decision;
    UmiStatus status;
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->storageFault) return UMI_STATUS_INVALID_STATE;
    status = UmiEnterpriseWorkspaceCheckAccess(workspace, actor, capability, resource, &decision);
    return status != UMI_STATUS_OK ? status : (decision.allowed ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED);
}
UmiStatus UmiEnterpriseWorkspaceOpen(UmiDataServer *data,
    UmiAuthorisationService *authorisation, UmiEnterpriseWorkspace **outWorkspace)
{
    UmiEnterpriseWorkspace *workspace;
    UmiStatus status;
    if (outWorkspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outWorkspace = NULL;
    if (data == NULL || authorisation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workspace = calloc(1U, sizeof(*workspace));
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workspace->data = data; workspace->authorisation = authorisation;
    workspace->state = calloc(1U, sizeof(*workspace->state));
    if (workspace->state == NULL) { free(workspace); return UMI_STATUS_OUT_OF_MEMORY; }
    status = EwsLoad(workspace, workspace->state, workspace->head);
    if (status != UMI_STATUS_OK) { UmiEnterpriseWorkspaceDestroy(workspace); return status; }
    *outWorkspace = workspace;
    return UMI_STATUS_OK;
}
void UmiEnterpriseWorkspaceDestroy(UmiEnterpriseWorkspace *workspace)
{
    if (workspace == NULL) return;
    free(workspace->state); free(workspace);
}
UmiStatus UmiEnterpriseWorkspaceReload(UmiEnterpriseWorkspace *workspace)
{
    EwsState *next;
    UmiStatus status;
    char head[EWS_HEAD_CAPACITY];
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->storageFault) return UMI_STATUS_INVALID_STATE;
    next = calloc(1U, sizeof(*next));
    if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = EwsLoad(workspace, next, head);
    if (status == UMI_STATUS_OK) {
        free(workspace->state); workspace->state = next;
        (void)EwsCopy(workspace->head, sizeof(workspace->head), head);
    } else free(next);
    return status;
}
UmiStatus UmiEnterpriseWorkspaceSnapshot(const UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseSnapshot *outSnapshot)
{
    UmiEnterpriseSnapshot snapshot = {0};
    const EwsState *state;
    if (workspace == NULL || outSnapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    state = workspace->state;
    snapshot.revision = state->revision; snapshot.datasetCount = state->datasetCount;
    snapshot.jobCount = state->jobCount; snapshot.auditCount = state->auditCount;
    snapshot.paused = state->paused; snapshot.storageFault = workspace->storageFault;
    for (size_t i = 0U; i < state->datasetCount; ++i) snapshot.rowCount += state->datasets[i].info.rowCount;
    for (size_t i = 0U; i < state->jobCount; ++i) {
        UmiEnterpriseJobState jobState = state->jobs[i].info.state;
        if (jobState == UMI_ENTERPRISE_JOB_REVIEW || jobState == UMI_ENTERPRISE_JOB_APPROVED) ++snapshot.pendingCount;
        if (jobState == UMI_ENTERPRISE_JOB_APPLIED) ++snapshot.appliedCount;
    }
    *outSnapshot = snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiEnterpriseWorkspaceDatasetAt(const UmiEnterpriseWorkspace *workspace,
    size_t index, UmiEnterpriseDataset *outDataset)
{
    if (workspace == NULL || outDataset == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= workspace->state->datasetCount) return UMI_STATUS_NOT_FOUND;
    *outDataset = workspace->state->datasets[index].info; return UMI_STATUS_OK;
}
UmiStatus UmiEnterpriseWorkspaceRowAt(const UmiEnterpriseWorkspace *workspace,
    const char *datasetId, size_t index, UmiEnterpriseRow *outRow)
{
    size_t found;
    if (workspace == NULL || !EwsId(datasetId) || outRow == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    found = EwsDatasetIndex(workspace->state, datasetId);
    if (found == SIZE_MAX || index >= workspace->state->datasets[found].info.rowCount) return UMI_STATUS_NOT_FOUND;
    *outRow = workspace->state->datasets[found].rows[index]; return UMI_STATUS_OK;
}
UmiStatus UmiEnterpriseWorkspaceJobAt(const UmiEnterpriseWorkspace *workspace,
    size_t index, UmiEnterpriseJob *outJob)
{
    if (workspace == NULL || outJob == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= workspace->state->jobCount) return UMI_STATUS_NOT_FOUND;
    *outJob = workspace->state->jobs[index].info; return UMI_STATUS_OK;
}
UmiStatus UmiEnterpriseWorkspaceJobFind(const UmiEnterpriseWorkspace *workspace,
    const char *jobId, UmiEnterpriseJob *outJob)
{
    size_t found;
    if (workspace == NULL || !EwsId(jobId) || outJob == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    found = EwsJobIndex(workspace->state, jobId);
    return found == SIZE_MAX ? UMI_STATUS_NOT_FOUND : UmiEnterpriseWorkspaceJobAt(workspace, found, outJob);
}
UmiStatus UmiEnterpriseWorkspaceJobPreview(const UmiEnterpriseWorkspace *workspace,
    const char *jobId, UmiEnterprisePreview *outPreview)
{
    size_t found;
    if (workspace == NULL || !EwsId(jobId) || outPreview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    found = EwsJobIndex(workspace->state, jobId);
    if (found == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *outPreview = workspace->state->jobs[found].preview;
    return UMI_STATUS_OK;
}
UmiStatus UmiEnterpriseWorkspaceAuditAt(const UmiEnterpriseWorkspace *workspace,
    size_t index, UmiEnterpriseAuditEntry *outEntry)
{
    if (workspace == NULL || outEntry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= workspace->state->auditCount) return UMI_STATUS_NOT_FOUND;
    *outEntry = workspace->state->audit[index]; return UMI_STATUS_OK;
}
UmiStatus UmiEnterpriseWorkspaceRecipeAt(const UmiEnterpriseWorkspace *workspace,
    size_t index, UmiEnterpriseRecipe *outRecipe)
{
    if (workspace == NULL || outRecipe == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= UMI_ENTERPRISE_RECIPE_COUNT) return UMI_STATUS_NOT_FOUND;
    EwsRecipe(index, outRecipe); outRecipe->enabled = workspace->state->recipes[index];
    return UMI_STATUS_OK;
}

/* Changes are applied to an owned candidate. Neither failed validation nor a
 * failed storage commit can partially publish rows or consume an approval. */
static EwsState *Candidate(const UmiEnterpriseWorkspace *workspace)
{
    EwsState *next = malloc(sizeof(*next));
    if (next != NULL) *next = *workspace->state;
    return next;
}
UmiStatus EwsCommit(UmiEnterpriseWorkspace *workspace, EwsState *next,
    UmiEnterpriseActor actor, const char *action, const char *target, const char *detail)
{
    UmiStatus status;
    UmiEnterpriseAuditEntry *entry;
    if (next->auditCount >= UMI_ENTERPRISE_MAX_AUDIT || next->revision == UINT64_MAX) {
        free(next); return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++next->revision;
    entry = &next->audit[next->auditCount++]; entry->revision = next->revision;
    (void)EwsCopy(entry->principal, sizeof(entry->principal), actor.principal);
    (void)EwsCopy(entry->action, sizeof(entry->action), action);
    (void)EwsCopy(entry->target, sizeof(entry->target), target);
    (void)EwsCopy(entry->detail, sizeof(entry->detail), detail);
    status = EwsValidate(next);
    if (status == UMI_STATUS_OK) status = EwsSave(workspace, next);
    if (status == UMI_STATUS_OK) { free(workspace->state); workspace->state = next; }
    else free(next);
    return status;
}
UmiStatus UmiEnterpriseWorkspaceCreateDataset(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *datasetId, const char *name)
{
    UmiStatus status;
    EwsState *next;
    UmiEnterpriseDataset *dataset;
    size_t found;
    if (!EwsId(datasetId) || !EwsText(name, UMI_ENTERPRISE_LABEL_CAPACITY, false, false)) return UMI_STATUS_INVALID_ARGUMENT;
    status = Permit(workspace, actor, "enterprise.dataset.create", datasetId);
    if (status != UMI_STATUS_OK) return status;
    found = EwsDatasetIndex(workspace->state, datasetId);
    if (found != SIZE_MAX) return strcmp(workspace->state->datasets[found].info.name, name) == 0
        ? EwsCurrent(workspace) : UMI_STATUS_ALREADY_EXISTS;
    if (workspace->state->datasetCount >= UMI_ENTERPRISE_MAX_DATASETS) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = Candidate(workspace); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    dataset = &next->datasets[next->datasetCount++].info;
    (void)EwsCopy(dataset->id, sizeof(dataset->id), datasetId);
    (void)EwsCopy(dataset->name, sizeof(dataset->name), name);
    dataset->generation = 1U;
    return EwsCommit(workspace, next, actor, "dataset.create", datasetId, name);
}
UmiStatus UmiEnterpriseWorkspaceSetRecipe(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *recipeId, bool enabled)
{
    size_t recipe = EwsRecipeIndex(recipeId);
    EwsState *next;
    UmiStatus status;
    if (recipe == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    status = Permit(workspace, actor, "enterprise.recipe.manage", recipeId);
    if (status != UMI_STATUS_OK) return status;
    if (workspace->state->recipes[recipe] == enabled) return EwsCurrent(workspace);
    next = Candidate(workspace); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    next->recipes[recipe] = enabled;
    return EwsCommit(workspace, next, actor, "recipe.set", recipeId, enabled ? "Enabled" : "Disabled");
}
UmiStatus UmiEnterpriseWorkspaceSetPaused(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, bool paused)
{
    EwsState *next;
    UmiStatus status = Permit(workspace, actor, "enterprise.execution.manage", "workspace");
    if (status != UMI_STATUS_OK) return status;
    if (workspace->state->paused == paused) return EwsCurrent(workspace);
    next = Candidate(workspace); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    next->paused = paused;
    return EwsCommit(workspace, next, actor, "execution.set", "workspace", paused ? "Paused" : "Resumed");
}
UmiStatus UmiEnterpriseWorkspacePreview(const UmiEnterpriseWorkspace *workspace,
    const char *datasetId, const char *recipeId, const char *csv, size_t length,
    UmiEnterprisePreview *outPreview, UmiEnterpriseIssue *outIssue)
{
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return EwsPreview(workspace->state, datasetId, recipeId, csv, length, outPreview, outIssue);
}
UmiStatus UmiEnterpriseWorkspacePrepare(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *jobId, const char *datasetId,
    const char *recipeId, const char *csv, size_t length, UmiEnterpriseIssue *outIssue)
{
    UmiStatus status;
    EwsState *next;
    EwsJob *job;
    size_t found;
    if (outIssue != NULL) (void)memset(outIssue, 0, sizeof(*outIssue));
    if (!EwsId(jobId) || !EwsId(datasetId) || EwsRecipeIndex(recipeId) == SIZE_MAX ||
        csv == NULL || length == 0U || length >= UMI_ENTERPRISE_CSV_CAPACITY || !EwsUtf8(csv, length, true)) return UMI_STATUS_INVALID_ARGUMENT;
    status = Permit(workspace, actor, "enterprise.job.prepare", datasetId);
    if (status != UMI_STATUS_OK) return status;
    found = EwsJobIndex(workspace->state, jobId);
    if (found != SIZE_MAX) {
        const EwsJob *old = &workspace->state->jobs[found];
        return strcmp(old->info.author, actor.principal) == 0 && strcmp(old->info.datasetId, datasetId) == 0 &&
            strcmp(old->info.recipeId, recipeId) == 0 && strlen(old->csv) == length && memcmp(old->csv, csv, length) == 0
            ? EwsCurrent(workspace) : UMI_STATUS_ALREADY_EXISTS;
    }
    if (workspace->state->jobCount >= UMI_ENTERPRISE_MAX_JOBS) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = Candidate(workspace); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    job = &next->jobs[next->jobCount];
    status = EwsPreview(next, datasetId, recipeId, csv, length, &job->preview, outIssue);
    if (status != UMI_STATUS_OK) { free(next); return status; }
    ++next->jobCount;
    (void)EwsCopy(job->info.id, sizeof(job->info.id), jobId);
    (void)EwsCopy(job->info.datasetId, sizeof(job->info.datasetId), datasetId);
    (void)EwsCopy(job->info.recipeId, sizeof(job->info.recipeId), recipeId);
    (void)EwsCopy(job->info.author, sizeof(job->info.author), actor.principal);
    (void)memcpy(job->csv, csv, length); job->csv[length] = '\0';
    job->info.state = UMI_ENTERPRISE_JOB_REVIEW;
    job->info.preparedRevision = next->revision + 1U;
    job->info.datasetGeneration = job->preview.datasetGeneration;
    job->info.rowCount = job->preview.rowCount;
    job->info.insertCount = job->preview.insertCount;
    job->info.updateCount = job->preview.updateCount;
    job->info.unchangedCount = job->preview.unchangedCount;
    return EwsCommit(workspace, next, actor, "job.prepare", jobId, "Frozen CSV and before/after rows saved for review");
}
UmiStatus UmiEnterpriseWorkspaceReview(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *jobId, bool approve, const char *reason)
{
    UmiStatus status;
    EwsState *next;
    EwsJob *job;
    size_t found, dataset;
    if (!EwsId(jobId) || !EwsText(reason, UMI_ENTERPRISE_LABEL_CAPACITY, false, false)) return UMI_STATUS_INVALID_ARGUMENT;
    status = Permit(workspace, actor, "enterprise.job.review", jobId);
    if (status != UMI_STATUS_OK) return status;
    found = EwsJobIndex(workspace->state, jobId);
    if (found == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    job = &workspace->state->jobs[found];
    if (strcmp(job->info.author, actor.principal) == 0) return UMI_STATUS_PERMISSION_DENIED;
    if (job->info.state != UMI_ENTERPRISE_JOB_REVIEW) return UMI_STATUS_INVALID_STATE;
    dataset = EwsDatasetIndex(workspace->state, job->info.datasetId);
    if (approve && workspace->state->datasets[dataset].info.generation != job->info.datasetGeneration) return UMI_STATUS_BUSY;
    next = Candidate(workspace); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    job = &next->jobs[found]; job->info.state = approve ? UMI_ENTERPRISE_JOB_APPROVED : UMI_ENTERPRISE_JOB_REJECTED;
    (void)EwsCopy(job->info.reviewer, sizeof(job->info.reviewer), actor.principal);
    (void)EwsCopy(job->info.reason, sizeof(job->info.reason), reason);
    job->info.decisionRevision = next->revision + 1U;
    return EwsCommit(workspace, next, actor, approve ? "job.approve" : "job.reject", jobId, reason);
}
UmiStatus UmiEnterpriseWorkspaceCancel(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *jobId, const char *reason)
{
    UmiStatus status;
    EwsState *next;
    EwsJob *job;
    size_t found;
    if (!EwsId(jobId) || !EwsText(reason, UMI_ENTERPRISE_LABEL_CAPACITY, false, false)) return UMI_STATUS_INVALID_ARGUMENT;
    status = Permit(workspace, actor, "enterprise.job.cancel", jobId);
    if (status != UMI_STATUS_OK) return status;
    found = EwsJobIndex(workspace->state, jobId);
    if (found == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    job = &workspace->state->jobs[found];
    if (strcmp(job->info.author, actor.principal) != 0) {
        status = Permit(workspace, actor, "enterprise.job.cancel.any", jobId);
        if (status != UMI_STATUS_OK) return status;
    }
    if (job->info.state != UMI_ENTERPRISE_JOB_REVIEW && job->info.state != UMI_ENTERPRISE_JOB_APPROVED) return UMI_STATUS_INVALID_STATE;
    next = Candidate(workspace); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    job = &next->jobs[found]; job->info.state = UMI_ENTERPRISE_JOB_CANCELLED;
    (void)EwsCopy(job->info.reviewer, sizeof(job->info.reviewer), actor.principal);
    (void)EwsCopy(job->info.reason, sizeof(job->info.reason), reason);
    job->info.decisionRevision = next->revision + 1U;
    return EwsCommit(workspace, next, actor, "job.cancel", jobId, reason);
}
UmiStatus UmiEnterpriseWorkspaceExecute(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *jobId)
{
    UmiStatus status;
    EwsState *next;
    EwsJob *job;
    EwsDataset *dataset;
    size_t found, datasetIndex;
    if (!EwsId(jobId)) return UMI_STATUS_INVALID_ARGUMENT;
    status = Permit(workspace, actor, "enterprise.job.execute", jobId);
    if (status != UMI_STATUS_OK) return status;
    found = EwsJobIndex(workspace->state, jobId);
    if (found == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    job = &workspace->state->jobs[found];
    if (job->info.state == UMI_ENTERPRISE_JOB_APPLIED) return EwsCurrent(workspace);
    if (job->info.state != UMI_ENTERPRISE_JOB_APPROVED) return UMI_STATUS_INVALID_STATE;
    if (workspace->state->paused || !workspace->state->recipes[EwsRecipeIndex(job->info.recipeId)]) return UMI_STATUS_PERMISSION_DENIED;
    datasetIndex = EwsDatasetIndex(workspace->state, job->info.datasetId);
    if (workspace->state->datasets[datasetIndex].info.generation != job->info.datasetGeneration) return UMI_STATUS_BUSY;
    next = Candidate(workspace); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    job = &next->jobs[found]; dataset = &next->datasets[datasetIndex];
    for (size_t i = 0U; i < job->preview.rowCount; ++i) {
        const UmiEnterpriseChange *change = &job->preview.changes[i];
        size_t rowIndex = SIZE_MAX;
        if (change->kind == UMI_ENTERPRISE_UNCHANGED) continue;
        for (size_t j = 0U; j < dataset->info.rowCount; ++j)
            if (strcmp(dataset->rows[j].id, change->after.id) == 0) { rowIndex = j; break; }
        if (rowIndex == SIZE_MAX) {
            if (dataset->info.rowCount >= UMI_ENTERPRISE_MAX_ROWS) { free(next); return UMI_STATUS_CAPACITY_EXCEEDED; }
            rowIndex = dataset->info.rowCount++;
        }
        dataset->rows[rowIndex] = change->after;
        (void)EwsCopy(dataset->rows[rowIndex].sourceJob, sizeof(dataset->rows[rowIndex].sourceJob), jobId);
    }
    if (job->info.insertCount + job->info.updateCount > 0U) ++dataset->info.generation;
    job->info.state = UMI_ENTERPRISE_JOB_APPLIED;
    job->info.appliedRevision = next->revision + 1U;
    (void)EwsCopy(job->info.executor, sizeof(job->info.executor), actor.principal);
    return EwsCommit(workspace, next, actor, "job.apply", jobId, "All reviewed rows and provenance committed together");
}
const char *UmiEnterpriseJobStateText(UmiEnterpriseJobState state)
{
    switch (state) {
    case UMI_ENTERPRISE_JOB_REVIEW: return "Review required";
    case UMI_ENTERPRISE_JOB_APPROVED: return "Approved";
    case UMI_ENTERPRISE_JOB_APPLIED: return "Applied";
    case UMI_ENTERPRISE_JOB_REJECTED: return "Rejected";
    case UMI_ENTERPRISE_JOB_CANCELLED: return "Cancelled";
    default: return "Invalid state";
    }
}
