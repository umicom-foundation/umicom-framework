/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/enterprise_workspace/validation.c
 *
 * PURPOSE:
 *   Reject inconsistent saved plans and provenance before publishing a workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <string.h>

static bool RowValid(const UmiEnterpriseRow *row, bool originRequired)
{
    return EwsId(row->id) && EwsText(row->label, sizeof(row->label), false, true) &&
        row->quantity <= (uint64_t)INT64_MAX &&
        (originRequired ? EwsId(row->sourceJob) : EwsText(row->sourceJob, sizeof(row->sourceJob), true, false));
}
static bool SameValue(const UmiEnterpriseRow *left, const UmiEnterpriseRow *right)
{
    return strcmp(left->id, right->id) == 0 && strcmp(left->label, right->label) == 0 && left->quantity == right->quantity;
}
static bool Origin(const EwsState *state, const char *datasetId, const UmiEnterpriseRow *row)
{
    size_t jobIndex = EwsJobIndex(state, row->sourceJob);
    const EwsJob *job;
    if (jobIndex == SIZE_MAX) return false;
    job = &state->jobs[jobIndex];
    if (job->info.state != UMI_ENTERPRISE_JOB_APPLIED || strcmp(job->info.datasetId, datasetId) != 0) return false;
    for (size_t i = 0U; i < job->preview.rowCount; ++i)
        if (SameValue(row, &job->preview.changes[i].after) && job->preview.changes[i].kind != UMI_ENTERPRISE_UNCHANGED) return true;
    return false;
}
UmiStatus EwsValidate(const EwsState *state)
{
    UmiEnterpriseRow parsed[UMI_ENTERPRISE_MAX_IMPORT_ROWS];
    if (state == NULL || state->datasetCount > UMI_ENTERPRISE_MAX_DATASETS || state->jobCount > UMI_ENTERPRISE_MAX_JOBS ||
        state->auditCount > UMI_ENTERPRISE_MAX_AUDIT || state->revision != state->auditCount) return UMI_STATUS_PARSE_ERROR;
    if (state->revision == 0U && (state->datasetCount != 0U || state->jobCount != 0U || state->paused || state->recipes[0] || state->recipes[1])) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < state->datasetCount; ++i)
        if (!EwsId(state->datasets[i].info.id)) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < state->jobCount; ++i)
        if (!EwsId(state->jobs[i].info.id)) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < state->auditCount; ++i) {
        const UmiEnterpriseAuditEntry *entry = &state->audit[i];
        if (entry->revision != i + 1U || !EwsId(entry->principal) || !EwsId(entry->action) || !EwsId(entry->target) ||
            !EwsText(entry->detail, sizeof(entry->detail), false, false)) return UMI_STATUS_PARSE_ERROR;
    }
    for (size_t i = 0U; i < state->datasetCount; ++i) {
        const EwsDataset *dataset = &state->datasets[i];
        if (!EwsId(dataset->info.id) || !EwsText(dataset->info.name, sizeof(dataset->info.name), false, false) ||
            dataset->info.rowCount > UMI_ENTERPRISE_MAX_ROWS || dataset->info.generation == 0U ||
            dataset->info.generation > state->revision || EwsDatasetIndex(state, dataset->info.id) != i) return UMI_STATUS_PARSE_ERROR;
        for (size_t j = 0U; j < dataset->info.rowCount; ++j) {
            if (!RowValid(&dataset->rows[j], true)) return UMI_STATUS_PARSE_ERROR;
            for (size_t k = 0U; k < j; ++k)
                if (strcmp(dataset->rows[k].id, dataset->rows[j].id) == 0) return UMI_STATUS_PARSE_ERROR;
        }
    }
    /* Validate all job IDs and counts before any cross-record lookup. */
    for (size_t i = 0U; i < state->jobCount; ++i) {
        const UmiEnterpriseJob *info = &state->jobs[i].info;
        if (!EwsId(info->id) || EwsJobIndex(state, info->id) != i || !EwsId(info->datasetId) ||
            !EwsId(info->recipeId) || !EwsId(info->author) || info->rowCount == 0U || info->rowCount > UMI_ENTERPRISE_MAX_IMPORT_ROWS ||
            state->jobs[i].preview.rowCount != info->rowCount) return UMI_STATUS_PARSE_ERROR;
    }
    for (size_t i = 0U; i < state->jobCount; ++i) {
        const EwsJob *job = &state->jobs[i];
        const UmiEnterpriseJob *info = &job->info;
        const UmiEnterprisePreview *preview = &job->preview;
        size_t recipe = EwsRecipeIndex(info->recipeId), dataset = EwsDatasetIndex(state, info->datasetId), count = 0U;
        size_t inserts = 0U, updates = 0U, unchanged = 0U;
        if (recipe == SIZE_MAX || dataset == SIZE_MAX || info->datasetGeneration == 0U ||
            info->datasetGeneration > state->datasets[dataset].info.generation ||
            !EwsText(info->reviewer, sizeof(info->reviewer), true, false) ||
            !EwsText(info->executor, sizeof(info->executor), true, false) ||
            !EwsText(info->reason, sizeof(info->reason), true, false) ||
            !EwsText(preview->datasetId, sizeof(preview->datasetId), false, false) ||
            !EwsText(preview->recipeId, sizeof(preview->recipeId), false, false) ||
            strcmp(preview->datasetId, info->datasetId) != 0 || strcmp(preview->recipeId, info->recipeId) != 0 ||
            preview->datasetGeneration != info->datasetGeneration || info->preparedRevision == 0U ||
            info->preparedRevision > state->revision || preview->workspaceRevision >= state->revision || preview->workspaceRevision + 1U != info->preparedRevision ||
            !EwsText(job->csv, sizeof(job->csv), false, true)) return UMI_STATUS_PARSE_ERROR;
        if (EwsParse(job->csv, strlen(job->csv), recipe, parsed, &count, NULL) != UMI_STATUS_OK || count != info->rowCount) return UMI_STATUS_PARSE_ERROR;
        for (size_t j = 0U; j < count; ++j) {
            const UmiEnterpriseChange *change = &preview->changes[j];
            if (!RowValid(&change->after, false) || change->after.sourceJob[0] != '\0' || !SameValue(&change->after, &parsed[j])) return UMI_STATUS_PARSE_ERROR;
            if (change->kind == UMI_ENTERPRISE_INSERT) {
                if (change->before.id[0] != '\0' || change->before.label[0] != '\0' || change->before.sourceJob[0] != '\0' || change->before.quantity != 0U) return UMI_STATUS_PARSE_ERROR;
                ++inserts;
            } else {
                if (!RowValid(&change->before, true) || strcmp(change->before.id, change->after.id) != 0) return UMI_STATUS_PARSE_ERROR;
                if (change->kind == UMI_ENTERPRISE_UPDATE && !SameValue(&change->before, &change->after)) ++updates;
                else if (change->kind == UMI_ENTERPRISE_UNCHANGED && SameValue(&change->before, &change->after)) ++unchanged;
                else return UMI_STATUS_PARSE_ERROR;
            }
        }
        if (inserts != info->insertCount || updates != info->updateCount || unchanged != info->unchangedCount ||
            inserts != preview->insertCount || updates != preview->updateCount || unchanged != preview->unchangedCount) return UMI_STATUS_PARSE_ERROR;
        if (info->state == UMI_ENTERPRISE_JOB_REVIEW) {
            if (info->reviewer[0] != '\0' || info->reason[0] != '\0' || info->decisionRevision != 0U) return UMI_STATUS_PARSE_ERROR;
        } else {
            if (info->state < UMI_ENTERPRISE_JOB_APPROVED || info->state > UMI_ENTERPRISE_JOB_CANCELLED ||
                !EwsId(info->reviewer) || info->reason[0] == '\0' || info->decisionRevision <= info->preparedRevision ||
                info->decisionRevision > state->revision) return UMI_STATUS_PARSE_ERROR;
            if (info->state != UMI_ENTERPRISE_JOB_CANCELLED && strcmp(info->reviewer, info->author) == 0) return UMI_STATUS_PARSE_ERROR;
        }
        if (info->state == UMI_ENTERPRISE_JOB_APPLIED) {
            if (!EwsId(info->executor) || info->appliedRevision <= info->decisionRevision || info->appliedRevision > state->revision ||
                (inserts + updates != 0U && state->datasets[dataset].info.generation <= info->datasetGeneration)) return UMI_STATUS_PARSE_ERROR;
        } else if (info->appliedRevision != 0U || info->executor[0] != '\0') return UMI_STATUS_PARSE_ERROR;
    }
    for (size_t i = 0U; i < state->datasetCount; ++i)
        for (size_t j = 0U; j < state->datasets[i].info.rowCount; ++j)
            if (!Origin(state, state->datasets[i].info.id, &state->datasets[i].rows[j])) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < state->jobCount; ++i)
        for (size_t j = 0U; j < state->jobs[i].preview.rowCount; ++j)
            if (state->jobs[i].preview.changes[j].kind != UMI_ENTERPRISE_INSERT &&
                !Origin(state, state->jobs[i].info.datasetId, &state->jobs[i].preview.changes[j].before)) return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}
