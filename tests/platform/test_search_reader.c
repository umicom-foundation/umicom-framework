/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_search_reader.c
 * PURPOSE: Exercise injected search reader ownership, refusal, cancellation and worker descriptor lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "search_reader_fixture.h"
typedef struct ReaderFixture
{
    SearchFixture files;
    const char *mode;
    UmiSearchFileReader *original;
    size_t reads, releases, matches;
    UmiSearchMatch match;
} ReaderFixture;
static UmiStatus Read(void *context, const char *path, size_t limit, const UmiCancellationToken *cancel,
                      unsigned char **out, size_t *bytes)
{
    ReaderFixture *f = context;
    (void)path;
    (void)limit;
    (void)cancel;
    ++f->reads;
    *out = NULL;
    *bytes = 0U;
    if (strcmp(f->mode, "descriptor-copy") == 0)
    {
        f->original->read = NULL;
        f->original->release = NULL;
    }
    if (strcmp(f->mode, "empty") == 0)
        return UMI_STATUS_OK;
    if (strcmp(f->mode, "invalid-buffer") == 0)
    {
        *bytes = 2U;
        return UMI_STATUS_OK;
    }
    const char *text = strcmp(f->mode, "literal-bom") == 0 ? "\xef\xbb\xbfneedle" : "needle needle";
    *bytes = strlen(text);
    *out = malloc(*bytes);
    if (*out == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(*out, text, *bytes);
    if (strcmp(f->mode, "binary") == 0)
        (*out)[0] = 0U;
    if (strcmp(f->mode, "unsupported") == 0)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (strcmp(f->mode, "failed-allocation") == 0)
        return UMI_STATUS_OUT_OF_MEMORY;
    if (strcmp(f->mode, "io-error") == 0 || strcmp(f->mode, "session-error") == 0)
        return UMI_STATUS_IO_ERROR;
    if (strcmp(f->mode, "cancel-reader") == 0)
        (void)umi_cancellation_token_request(f->files.cancel);
    return UMI_STATUS_OK;
}
static void Release(void *context, void *text)
{
    ReaderFixture *f = context;
    ++f->releases;
    free(text);
}
static UmiStatus Match(const UmiSearchMatch *match, void *context)
{
    ReaderFixture *f = context;
    f->match = *match;
    ++f->matches;
    if (strcmp(f->mode, "sink-error") == 0)
        return UMI_STATUS_IO_ERROR;
    if (strcmp(f->mode, "cancel-sink") == 0)
        (void)umi_cancellation_token_request(f->files.cancel);
    if (strcmp(f->mode, "index-changed") == 0)
        (void)umi_file_index_clear(f->files.index);
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *cases[] = {"owned",           "empty",
                                            "invalid-buffer",  "binary",
                                            "oversized",       "serialized-limit",
                                            "unsupported",     "failed-allocation",
                                            "io-error",        "sink-error",
                                            "cancel-before",   "cancel-reader",
                                            "cancel-sink",     "index-changed",
                                            "excluded",        "no-read",
                                            "no-release",      "no-limit",
                                            "descriptor-copy", "session-copy",
                                            "session-error",   "raw-fallback",
                                            "literal-bom",     "result-limit"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReaderFixture f = {0};
    f.mode = mode;
    CHECK(SearchFixtureOpen(&f.files, "reader", mode) == 0);
    CHECK(SearchFixturePut(&f.files, "note.txt", "disk needle", 11U) == 0);
    CHECK(SearchFixtureIndex(&f.files) == 0);
    UmiSearchFileReader reader = {&f, 64U, Read, Release};
    f.original = &reader;
    UmiSearchRequest request = umi_search_request_default("needle");
    request.cancellation = f.files.cancel;
    UmiSearchPathFilter filter;
    CHECK(UmiSearchPathFilterInit("", "", &filter) == UMI_STATUS_OK);
    if (strcmp(mode, "excluded") == 0)
        CHECK(UmiSearchPathFilterInit("*.c", "", &filter) == UMI_STATUS_OK);
    if (strcmp(mode, "serialized-limit") == 0)
        reader.maximum_serialized_bytes = 1U;
    if (strcmp(mode, "oversized") == 0)
        request.maximum_file_size = 4U;
    if (strcmp(mode, "no-read") == 0)
        reader.read = NULL;
    if (strcmp(mode, "no-release") == 0)
        reader.release = NULL;
    if (strcmp(mode, "no-limit") == 0)
        reader.maximum_serialized_bytes = 0U;
    if (strcmp(mode, "cancel-before") == 0)
        (void)umi_cancellation_token_request(f.files.cancel);
    if (strcmp(mode, "result-limit") == 0)
        request.maximum_results = 1U;
    UmiSearchStats stats;
    UmiStatus status;
    if (strncmp(mode, "session-", 8U) == 0)
    {
        CHECK(UmiFileSearchCreateWithReader(f.files.index, &reader, &f.files.session) == UMI_STATUS_OK);
        reader.read = NULL;
        reader.release = NULL;
        reader.maximum_serialized_bytes = 0U;
        CHECK(UmiFileSearchStart(f.files.session, "needle", 1, 0U) == UMI_STATUS_OK);
        CHECK(UmiFileSearchWait(f.files.session, 10000U) == UMI_STATUS_OK);
        UmiFileSearchSnapshot state;
        CHECK(UmiFileSearchRead(f.files.session, &state) == UMI_STATUS_OK);
        stats = state.stats;
        status = state.status;
        if (strcmp(mode, "session-copy") == 0)
        {
            CHECK(state.ready && stats.matches == 2U);
            CHECK(UmiFileSearchMatchAt(f.files.session, state.requestId, 0U, &f.match) == UMI_STATUS_OK);
        }
        else
        {
            CHECK(!state.ready);
            CHECK(UmiFileSearchMatchAt(f.files.session, state.requestId, 0U, &f.match) != UMI_STATUS_OK);
        }
    }
    else
        status = UmiSearchFileIndexWithReader(f.files.index, &request, NULL, &filter,
                                              strcmp(mode, "raw-fallback") == 0 ? NULL : &reader, Match, &f,
                                              &stats);
    UmiStatus wanted = UMI_STATUS_OK;
    if (strcmp(mode, "invalid-buffer") == 0)
        wanted = UMI_STATUS_INVALID_STATE;
    if (strcmp(mode, "failed-allocation") == 0)
        wanted = UMI_STATUS_OUT_OF_MEMORY;
    if (strcmp(mode, "io-error") == 0 || strcmp(mode, "sink-error") == 0 ||
        strcmp(mode, "session-error") == 0)
        wanted = UMI_STATUS_IO_ERROR;
    if (strncmp(mode, "cancel-", 7U) == 0)
        wanted = UMI_STATUS_CANCELLED;
    if (strcmp(mode, "index-changed") == 0)
        wanted = UMI_STATUS_BUSY;
    if (strncmp(mode, "no-", 3U) == 0)
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    CHECK(status == wanted);
    int unread = strcmp(mode, "excluded") == 0 || strcmp(mode, "serialized-limit") == 0 ||
                 strcmp(mode, "cancel-before") == 0 || strncmp(mode, "no-", 3U) == 0 ||
                 strcmp(mode, "raw-fallback") == 0;
    CHECK(f.reads == (unread ? 0U : 1U));
    CHECK(f.releases ==
          (unread || strcmp(mode, "empty") == 0 || strcmp(mode, "invalid-buffer") == 0 ? 0U : 1U));
    if (strcmp(mode, "binary") == 0 || strcmp(mode, "unsupported") == 0)
        CHECK(stats.binary_files_skipped == 1U && stats.matches == 0U);
    if (strcmp(mode, "oversized") == 0 || strcmp(mode, "serialized-limit") == 0)
        CHECK(stats.oversized_files_skipped == 1U && stats.matches == 0U);
    if (strcmp(mode, "owned") == 0 || strcmp(mode, "descriptor-copy") == 0)
        CHECK(stats.matches == 2U && f.matches == 2U);
    if (strcmp(mode, "literal-bom") == 0)
        CHECK(f.match.column == 4U);
    if (strcmp(mode, "result-limit") == 0)
        CHECK(stats.truncated && stats.matches == 1U);
    if (strcmp(mode, "raw-fallback") == 0)
        CHECK(stats.matches == 1U && f.match.column == 6U);
    if (strncmp(mode, "cancel-", 7U) == 0)
        CHECK(stats.cancelled);
    SearchFixtureStop(&f.files);
    return 0;
}
