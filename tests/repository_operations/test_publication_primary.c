/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_publication_primary.c
 *
 * PURPOSE:
 *   Verify explicit repository ownership without fetching, staging or publishing.
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

#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #value); result = EXIT_FAILURE; goto done; \
} } while (0)

static UmiStatus preview_lock(void *context, const char *path, int dry_run)
{
    size_t *calls = context;
    (void)path;
    if (!dry_run) return UMI_STATUS_PERMISSION_DENIED;
    ++*calls;
    return UMI_STATUS_OK;
}

static UmiStatus forbidden_git(void *context, const char *path,
    const char *const *args, size_t count, UmiPublicationCapture *capture)
{
    size_t *calls = context;
    (void)path; (void)args; (void)count;
    memset(capture, 0, sizeof(*capture));
    ++*calls;
    return UMI_STATUS_PERMISSION_DENIED;
}

static void fixture(UmiPublicationState *state, UmiRepositoryPublicationReport *report,
    size_t *calls)
{
    static const char *const paths[] = {
        "/work/OS/framework", "/work/Applications", "/work/Applications/framework", "/work/kernel"
    };
    static const char *const origins[] = {"host/framework", "host/apps", "host/framework", "host/kernel"};
    memset(state, 0, sizeof(*state));
    memset(report, 0, sizeof(*report));
    state->report = report;
    state->backend.context = calls;
    state->backend.lock = preview_lock;
    (void)snprintf(state->root, sizeof(state->root), "/work");
    (void)snprintf(state->primary_root, sizeof(state->primary_root), "/work/Applications");
    report->count = 4U;
    for (size_t i = 0U; i < report->count; ++i) {
        UmiRepositoryPublicationEntry *entry = &report->entries[i];
        (void)snprintf(entry->path, sizeof(entry->path), "%s", paths[i]);
        (void)snprintf(state->identity[i], sizeof(state->identity[i]), "%s", origins[i]);
        memset(entry->head, 'a', 40U); memset(entry->remote_head, 'a', 40U);
        entry->parent = SIZE_MAX; entry->primary = i;
    }
    state->edges[0].parent = 1U; state->edges[0].child = 2U;
    state->edge_count = 1U;
}

int main(int argc, char **argv)
{
    UmiPublicationState *state = calloc(1U, sizeof(*state));
    UmiRepositoryPublicationReport *report = calloc(1U, sizeof(*report));
    size_t calls = 0U;
    int result = EXIT_SUCCESS;
    CHECK(state && report && argc == 2);
    fixture(state, report, &calls);
    if (strcmp(argv[1], "default-options") == 0) {
        UmiRepositoryPublicationOptions options;
        memset(&options, 0x7f, sizeof(options));
        umi_repository_publication_options_init(&options);
        CHECK(!options.primary_directory && !options.attach_published_main &&
            !options.review_staged && !options.review_context);
        umi_repository_publication_options_init(NULL);
    } else if (strcmp(argv[1], "size-contract") == 0) {
        UmiRepositoryPublicationOptions options = {0};
        report->count = 71U;
        CHECK(umi_repository_publication_run_options_sized(NULL, NULL, NULL,
            &options, sizeof(options) - 1U, report, sizeof(*report)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report->count == 71U);
        CHECK(umi_repository_publication_run_options_sized(NULL, NULL, NULL,
            &options, sizeof(options), report, sizeof(*report) - 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report->count == 71U);
    } else if (strcmp(argv[1], "outside-workspace") == 0 ||
               strcmp(argv[1], "empty-directory") == 0) {
        UmiRepositoryPublicationRequest request;
        UmiRepositoryPublicationOptions options = {0};
        UmiPublicationBackend backend = {&calls, forbidden_git, preview_lock};
        umi_repository_publication_request_init(&request, "/work");
        options.primary_directory = strcmp(argv[1], "empty-directory") == 0 ? "" : "../outside";
        CHECK(umi_publication_run_options_backend(NULL, NULL, &request, &options,
            report, &backend) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(calls == 0U);
    } else {
        UmiStatus expected = UMI_STATUS_OK;
        if (strcmp(argv[1], "clean-primary") == 0) {
            /* The first discovered copy must no longer win a clean tie. */
        } else if (strcmp(argv[1], "edited-primary") == 0) {
            report->entries[2].changed_paths = 2U;
        } else if (strcmp(argv[1], "edited-copy") == 0) {
            report->entries[0].changed_paths = 2U; expected = UMI_STATUS_INVALID_STATE;
        } else if (strcmp(argv[1], "unpublished-copy") == 0) {
            report->entries[0].ahead = 1U; report->entries[0].head[0] = 'b';
            expected = UMI_STATUS_INVALID_STATE;
        } else if (strcmp(argv[1], "identical-unpublished") == 0) {
            report->entries[0].ahead = report->entries[2].ahead = 1U;
            report->entries[0].head[0] = report->entries[2].head[0] = 'b';
        } else if (strcmp(argv[1], "ambiguous-primary") == 0) {
            (void)snprintf(report->entries[0].path, sizeof(report->entries[0].path),
                "/work/Applications/another-framework"); expected = UMI_STATUS_INVALID_STATE;
        } else if (strcmp(argv[1], "missing-directory") == 0) {
            (void)snprintf(state->primary_root, sizeof(state->primary_root), "/work/missing");
            expected = UMI_STATUS_INVALID_ARGUMENT;
        } else if (strcmp(argv[1], "boundary-directory") == 0) {
            (void)snprintf(state->primary_root, sizeof(state->primary_root), "/work/App");
            expected = UMI_STATUS_INVALID_ARGUMENT;
        } else if (strcmp(argv[1], "other-repository") == 0) {
            report->entries[3].changed_paths = 1U;
        } else if (strcmp(argv[1], "remote-mismatch") == 0) {
            report->entries[0].remote_head[0] = 'b'; expected = UMI_STATUS_INVALID_STATE;
        } else if (strcmp(argv[1], "compatibility") == 0) {
            state->primary_root[0] = '\0'; report->entries[0].changed_paths = 1U;
        } else if (strcmp(argv[1], "vendor-only") == 0) {
            report->entries[1].vendor = report->entries[2].vendor = 1;
            expected = UMI_STATUS_INVALID_ARGUMENT;
        } else {
            CHECK(!"Unknown fixture case");
        }
        CHECK(umi_publication_plan(state) == expected);
        if (expected != UMI_STATUS_OK) {
            CHECK(report->blocked_count == (expected == UMI_STATUS_INVALID_ARGUMENT ? 0U : 1U));
            CHECK(report->detail[0] && calls == 0U);
        } else {
            size_t primary = strcmp(argv[1], "compatibility") == 0 ? 0U : 2U;
            CHECK(report->entries[0].primary == primary && report->entries[2].primary == primary);
            CHECK(report->entries[3].primary == 3U && report->repository_count == 3U);
            size_t child = SIZE_MAX, parent = SIZE_MAX;
            for (size_t i = 0U; i < state->order_count; ++i) {
                if (state->order[i] == primary) child = i;
                if (state->order[i] == 1U) parent = i;
            }
            CHECK(child < parent && calls == 1U);
        }
    }
done:
    free(state); free(report);
    return result;
}
