/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_publication_review.c
 *
 * PURPOSE:
 *   Verify staged snapshot review and refusal without executing Git or creating commits.
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

typedef struct ReviewFixture {
    const char *scenario;
    size_t calls;
    size_t reviews;
    char head[65];
    char tree[65];
    UmiCancellationToken *cancellation;
} ReviewFixture;

static UmiStatus output(UmiPublicationCapture *capture, const char *text, size_t size)
{
    capture->bytes = malloc(size + 1U);
    if (!capture->bytes) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(capture->bytes, text, size);
    capture->bytes[size] = '\0';
    capture->size = size;
    capture->exit_code = 0;
    return UMI_STATUS_OK;
}

static UmiStatus fake_git(void *context, const char *path,
    const char *const *args, size_t count, UmiPublicationCapture *capture)
{
    ReviewFixture *fixture = context;
    ++fixture->calls;
    if (strcmp(path, "/work/source") != 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (count == 1U && strcmp(args[0], "write-tree") == 0) {
        if (strcmp(fixture->scenario, "tree-probe-failure") == 0) return UMI_STATUS_IO_ERROR;
        const char *tree = strcmp(fixture->scenario, "changed-index") == 0
            ? "dddddddddddddddddddddddddddddddddddddddd" : fixture->tree;
        return output(capture, tree, strlen(tree));
    }
    int patch = count == 9U;
    const char *const expected[] = {"diff", "--no-color", "--no-ext-diff", "--no-textconv"};
    if (count != 8U && count != 9U) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < 4U; ++i)
        if (strcmp(args[i], expected[i]) != 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(args[4], patch ? "--patch" : "--stat") != 0 ||
        (patch && strcmp(args[5], "--binary") != 0) ||
        strcmp(args[count - 3U], fixture->head) != 0 ||
        strcmp(args[count - 2U], fixture->tree) != 0 ||
        strcmp(args[count - 1U], "--") != 0) return UMI_STATUS_INVALID_ARGUMENT;
    if ((!patch && strcmp(fixture->scenario, "summary-failure") == 0) ||
        (patch && strcmp(fixture->scenario, "patch-failure") == 0)) return UMI_STATUS_IO_ERROR;
    if ((!patch && strcmp(fixture->scenario, "summary-nul") == 0) ||
        (patch && strcmp(fixture->scenario, "patch-nul") == 0))
        return output(capture, "hidden\0content", 14U);
    if ((!patch && strcmp(fixture->scenario, "summary-empty") == 0) ||
        (patch && strcmp(fixture->scenario, "patch-empty") == 0)) return output(capture, "", 0U);
    const char *text = patch ? "diff --git a/source.c b/source.c\n-old\n+new\n" : " source.c | 2 +-\n";
    return output(capture, text, strlen(text));
}

static UmiStatus review(const UmiRepositoryPublicationStagedReview *view, void *context)
{
    ReviewFixture *fixture = context;
    ++fixture->reviews;
    if (strcmp(view->repository_path, "/work/source") != 0 ||
        strcmp(view->head, fixture->head) != 0 || strcmp(view->tree, fixture->tree) != 0 ||
        view->summary_size != strlen(view->summary) ||
        view->patch_size != strlen(view->patch) || !strstr(view->patch, "+new\n"))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(fixture->scenario, "refused") == 0) return UMI_STATUS_CANCELLED;
    if (strcmp(fixture->scenario, "display-failure") == 0) return UMI_STATUS_IO_ERROR;
    if (strcmp(fixture->scenario, "cancel-during-review") == 0)
        umi_cancellation_token_request(fixture->cancellation);
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    UmiPublicationState *state = calloc(1U, sizeof(*state));
    UmiRepositoryPublicationReport *report = calloc(1U, sizeof(*report));
    ReviewFixture fixture = {0};
    UmiRepositoryPublicationRequest request = {0};
    int result = EXIT_SUCCESS;
    CHECK(state && report && argc == 2);
    CHECK(umi_cancellation_token_create(&fixture.cancellation) == UMI_STATUS_OK);
    request.cancellation = fixture.cancellation;
    state->request = &request;
    fixture.scenario = argv[1];
    size_t width = strcmp(argv[1], "sha256") == 0 ? 64U : 40U;
    memset(fixture.head, 'a', width); memset(fixture.tree, 'b', width);
    state->report = report; report->count = 1U;
    state->backend.context = &fixture; state->backend.git = fake_git;
    state->review_staged = review; state->review_context = &fixture;
    (void)snprintf(report->entries[0].path, sizeof(report->entries[0].path), "/work/source");
    (void)snprintf(report->entries[0].head, sizeof(report->entries[0].head), "%s", fixture.head);
    UmiStatus expected = UMI_STATUS_OK;
    size_t reviews = 1U, calls = 3U;
    if (strcmp(argv[1], "approved") == 0 || strcmp(argv[1], "sha256") == 0) {
    } else if (strcmp(argv[1], "refused") == 0) {
        expected = UMI_STATUS_CANCELLED; calls = 2U;
    } else if (strcmp(argv[1], "display-failure") == 0) {
        expected = UMI_STATUS_IO_ERROR; calls = 2U;
    } else if (strcmp(argv[1], "changed-index") == 0) {
        expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(argv[1], "tree-probe-failure") == 0) {
        expected = UMI_STATUS_IO_ERROR;
    } else if (strcmp(argv[1], "summary-failure") == 0 || strcmp(argv[1], "patch-failure") == 0) {
        expected = UMI_STATUS_IO_ERROR; reviews = 0U;
        calls = strcmp(argv[1], "summary-failure") == 0 ? 1U : 2U;
    } else if (strcmp(argv[1], "summary-nul") == 0 || strcmp(argv[1], "patch-nul") == 0 ||
        strcmp(argv[1], "summary-empty") == 0 || strcmp(argv[1], "patch-empty") == 0) {
        expected = UMI_STATUS_PARSE_ERROR; reviews = 0U; calls = 2U;
    } else if (strcmp(argv[1], "invalid-object") == 0) {
        fixture.tree[0] = '-'; expected = UMI_STATUS_PARSE_ERROR; reviews = calls = 0U;
    } else if (strcmp(argv[1], "cancel-before-review") == 0) {
        umi_cancellation_token_request(fixture.cancellation);
        expected = UMI_STATUS_CANCELLED; reviews = calls = 0U;
    } else if (strcmp(argv[1], "cancel-during-review") == 0) {
        expected = UMI_STATUS_CANCELLED; calls = 2U;
    } else if (strcmp(argv[1], "compatibility") == 0) {
        state->review_staged = NULL; reviews = calls = 0U;
    } else {
        CHECK(!"Unknown review fixture");
    }
    CHECK(umi_publication_review_staged(state, 0U, fixture.tree) == expected);
    CHECK(fixture.reviews == reviews && fixture.calls == calls);
    CHECK(report->committed_count == 0U && report->pushed_count == 0U);
    CHECK(report->blocked_count == (expected == UMI_STATUS_OK ? 0U : 1U));
    CHECK(strcmp(report->entries[0].head, fixture.head) == 0);
done:
    umi_cancellation_token_destroy(fixture.cancellation);
    free(state); free(report);
    return result;
}
