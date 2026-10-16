/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_service.c
 *
 * PURPOSE:
 *   Compose discovery, preflight and ordered publication as a reusable Framework service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void umi_repository_publication_request_init(
    UmiRepositoryPublicationRequest *request, const char *workspace_root)
{
    if (!request) return;
    memset(request, 0, sizeof(*request));
    request->workspace_root = workspace_root;
    request->refresh = 1;
    request->timeout_ms = 300000U;
}

static UmiStatus prepare(UmiPublicationState *state)
{
    UmiStatus first_failure = UMI_STATUS_OK;
    /* A refreshed graph can reorder checkouts; retain successful actions by
     * path before clearing observations so partial results remain truthful. */
    UmiStatus progress_status = umi_publication_remember_progress(state);
    if (progress_status != UMI_STATUS_OK) return progress_status;
    state->report->count = 0U;
    state->report->blocked_count = 0U;
    state->report->repository_count = 0U;
    state->report->detail[0] = '\0';
    memset(state->report->entries, 0, sizeof(state->report->entries));
    memset(state->identity, 0, sizeof(state->identity));
    state->edge_count = 0U;
    UmiStatus status = umi_publication_discover(state);
    /* Restore completed flags even if discovery only reached a partial graph. */
    umi_publication_restore_progress(state);
    if (status != UMI_STATUS_OK) return status;
    /* Continue read-only inspection to show all checkout problems in one
     * report, but never enter execution if any repository failed preflight. */
    for (size_t i = 0U; i < state->report->count; ++i) {
        if (state->request->cancellation &&
            umi_cancellation_token_is_requested(state->request->cancellation))
            return UMI_STATUS_CANCELLED;
        status = umi_publication_inspect(state, i);
        if (status != UMI_STATUS_OK) {
            if (first_failure == UMI_STATUS_OK) first_failure = status;
            if (!state->report->entries[i].blocked)
                (void)umi_publication_fail(state, i, status,
                    "Repository inspection was incomplete; publication stopped.");
        }
    }
    if (first_failure != UMI_STATUS_OK) return first_failure;
    return umi_publication_plan(state);
}

static UmiStatus verify_completion(UmiPublicationState *state)
{
    for (size_t i = 0U; i < state->report->count; ++i) {
        UmiRepositoryPublicationEntry *entry = &state->report->entries[i];
        char expected[65];
        (void)snprintf(expected, sizeof(expected), "%s", entry->head);
        UmiStatus status = umi_publication_inspect(state, i);
        if (status != UMI_STATUS_OK) return status;
        if (entry->vendor) continue;
        status = umi_publication_verify_links(state, i);
        if (status != UMI_STATUS_OK) return status;
        if (state->dirty[i] || entry->changed_paths || entry->ahead || entry->behind ||
            strcmp(entry->head, expected) != 0 ||
            strcmp(entry->head, state->report->entries[entry->primary].head) != 0)
            return umi_publication_fail(state, i, UMI_STATUS_INVALID_STATE,
                "Working files or remote revisions changed before completion; review the remaining changes.");
    }
    return UMI_STATUS_OK;
}

/* An explicit recovery entry point extends preflight while the original API remains a no-recovery wrapper. The former implementation is retained for review. */
#if 0
UmiStatus umi_publication_run_backend(const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment, const UmiRepositoryPublicationRequest *request,
    UmiRepositoryPublicationReport *report, const UmiPublicationBackend *backend)
{
    UmiPublicationState *state;
    char cwd[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (!report) return UMI_STATUS_INVALID_ARGUMENT;
    memset(report, 0, sizeof(*report));
    if (!request || !request->workspace_root || !request->workspace_root[0] ||
        (request->apply != 0 && request->apply != 1) ||
        (request->refresh != 0 && request->refresh != 1) ||
        (request->apply && !request->refresh) ||
        request->timeout_ms == 0U || request->timeout_ms > 3600000U ||
        (request->commit_message && (!request->commit_message[0] ||
          strlen(request->commit_message) > 512U ||
          strchr(request->commit_message, '\n') || strchr(request->commit_message, '\r')))) {
        report->status = UMI_STATUS_INVALID_ARGUMENT;
        return report->status;
    }
    if ((!backend && !profile) ||
        (backend && (!backend->git || !backend->lock))) {
        report->status = UMI_STATUS_INVALID_ARGUMENT;
        return report->status;
    }
    state = calloc(1U, sizeof(*state));
    if (!state) return report->status = UMI_STATUS_OUT_OF_MEMORY;
    state->profile = profile;
    state->environment = environment;
    state->request = request;
    state->report = report;
    state->backend = backend ? *backend : (UmiPublicationBackend){
        state, umi_publication_native_git, umi_publication_native_lock};
    status = umi_fs_current_directory(cwd, sizeof(cwd));
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(request->workspace_root, cwd, state->root, sizeof(state->root));
    if (status == UMI_STATUS_OK) status = prepare(state);
    if (status == UMI_STATUS_OK && request->apply) {
        /* Fast-forward clean old copies only after global preflight. Rebuild
         * the graph afterwards: incoming commits may register new children.
         * Limit refresh churn instead of chasing an actively moving remote. */
        for (unsigned pass = 0U; pass < 3U && status == UMI_STATUS_OK; ++pass) {
            int updated = 0;
            status = umi_publication_acquire_locks(state);
            if (status != UMI_STATUS_OK) break;
            status = umi_publication_update_initial(state, &updated);
            if (status != UMI_STATUS_OK) break;
            if (!updated) {
                status = umi_publication_execute(state);
                if (status == UMI_STATUS_OK) status = verify_completion(state);
                break;
            }
            status = prepare(state);
            if (status == UMI_STATUS_OK && pass == 2U)
                status = umi_publication_fail(state, SIZE_MAX, UMI_STATUS_BUSY,
                    "Remote revisions keep advancing; retry after other publishers finish.");
        }
    }
    UmiStatus release_status = umi_publication_release_locks(state);
    if (status == UMI_STATUS_OK && release_status != UMI_STATUS_OK)
        status = umi_publication_fail(state, SIZE_MAX, release_status,
            "Publication finished but an owned metadata marker could not be removed; inspect it before retrying.");
    report->status = status;
    if (report->detail[0] == '\0')
        (void)snprintf(report->detail, sizeof(report->detail), "%s",
            status != UMI_STATUS_OK ? "Publication did not complete; existing work is retained." :
            request->apply ? "Publication and final checkout checks completed." :
            request->refresh ? "Preview complete; fetched metadata only. No staging, commits or pushes." :
            "Local monitor snapshot; remote comparisons use cached refs. No Git mutations.");
    free(state);
    return status;
}

#endif
UmiStatus umi_publication_run_recovery_backend(const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment, const UmiRepositoryPublicationRequest *request,
    UmiRepositoryPublicationReport *report, const UmiPublicationBackend *backend, int attach_published_main)
{
    UmiPublicationState *state;
    char cwd[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (!report) return UMI_STATUS_INVALID_ARGUMENT;
    memset(report, 0, sizeof(*report));
    if ((attach_published_main != 0 && attach_published_main != 1) ||
        (attach_published_main && request && !request->refresh) || !request || !request->workspace_root || !request->workspace_root[0] ||
        (request->apply != 0 && request->apply != 1) ||
        (request->refresh != 0 && request->refresh != 1) ||
        (request->apply && !request->refresh) ||
        request->timeout_ms == 0U || request->timeout_ms > 3600000U ||
        (request->commit_message && (!request->commit_message[0] ||
          strlen(request->commit_message) > 512U ||
          strchr(request->commit_message, '\n') || strchr(request->commit_message, '\r')))) {
        report->status = UMI_STATUS_INVALID_ARGUMENT;
        return report->status;
    }
    if ((!backend && !profile) ||
        (backend && (!backend->git || !backend->lock))) {
        report->status = UMI_STATUS_INVALID_ARGUMENT;
        return report->status;
    }
    state = calloc(1U, sizeof(*state));
    if (!state) return report->status = UMI_STATUS_OUT_OF_MEMORY;
    state->profile = profile;
    state->environment = environment;
    state->request = request;
    state->attach_published_main = attach_published_main;
    state->report = report;
    state->backend = backend ? *backend : (UmiPublicationBackend){
        state, umi_publication_native_git, umi_publication_native_lock};
    status = umi_fs_current_directory(cwd, sizeof(cwd));
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(request->workspace_root, cwd, state->root, sizeof(state->root));
    if (status == UMI_STATUS_OK) status = prepare(state);
    if (status == UMI_STATUS_OK && request->apply) {
        /* Fast-forward clean old copies only after global preflight. Rebuild
         * the graph afterwards: incoming commits may register new children.
         * Limit refresh churn instead of chasing an actively moving remote. */
        for (unsigned pass = 0U; pass < 3U && status == UMI_STATUS_OK; ++pass) {
            int updated = 0;
            status = umi_publication_acquire_locks(state);
            if (status != UMI_STATUS_OK) break;
            status = umi_publication_update_initial(state, &updated);
            if (status != UMI_STATUS_OK) break;
            if (!updated) {
                status = umi_publication_execute(state);
                if (status == UMI_STATUS_OK) status = verify_completion(state);
                break;
            }
            status = prepare(state);
            if (status == UMI_STATUS_OK && pass == 2U)
                status = umi_publication_fail(state, SIZE_MAX, UMI_STATUS_BUSY,
                    "Remote revisions keep advancing; retry after other publishers finish.");
        }
    }
    UmiStatus release_status = umi_publication_release_locks(state);
    if (status == UMI_STATUS_OK && release_status != UMI_STATUS_OK)
        status = umi_publication_fail(state, SIZE_MAX, release_status,
            "Publication finished but an owned metadata marker could not be removed; inspect it before retrying.");
    report->status = status;
    if (report->detail[0] == '\0')
        (void)snprintf(report->detail, sizeof(report->detail), "%s",
            status != UMI_STATUS_OK ? "Publication did not complete; existing work is retained." :
            request->apply ? "Publication and final checkout checks completed." :
            request->refresh ? "Preview complete; fetched metadata only. No staging, commits or pushes." :
            "Local monitor snapshot; remote comparisons use cached refs. No Git mutations.");
    free(state);
    return status;
}


/* Existing callers retain the original no-attachment policy. */
UmiStatus umi_publication_run_backend(const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment, const UmiRepositoryPublicationRequest *request,
    UmiRepositoryPublicationReport *report, const UmiPublicationBackend *backend)
{
    return umi_publication_run_recovery_backend(profile, environment, request, report, backend, 0);
}

UmiStatus umi_repository_publication_run_sized(const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment, const UmiRepositoryPublicationRequest *request,
    UmiRepositoryPublicationReport *report, size_t report_size)
{
    if (!report || report_size != sizeof(*report)) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_publication_run_backend(profile, environment, request, report, NULL);
}

UmiStatus umi_repository_publication_run_recovery_sized(const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment, const UmiRepositoryPublicationRequest *request,
    UmiRepositoryPublicationReport *report, size_t report_size, int attach_published_main)
{
    if (!report || report_size != sizeof(*report)) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_publication_run_recovery_backend(profile, environment, request, report, NULL,
        attach_published_main);
}
