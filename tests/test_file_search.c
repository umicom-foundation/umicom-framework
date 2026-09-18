/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_file_search.c
 *
 * PURPOSE:
 *   Exercise saved-file search, bounded reads, revision checks, cancellation
 *   and task ownership using real temporary files and the production services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/search_session.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return EXIT_FAILURE; } } while (0)

typedef struct Fixture {
    UmiFileIndex *index;
    UmiCancellationToken *token;
    UmiSearchMatch matches[16];
    size_t count;
    int cancelAfterMatch;
    int changeAfterMatch;
    int failSink;
} Fixture;

static UmiStatus Match(const UmiSearchMatch *match, void *userData)
{
    Fixture *fixture = userData;
    if (fixture->failSink) return UMI_STATUS_IO_ERROR;
    if (fixture->count < 16U) fixture->matches[fixture->count] = *match;
    ++fixture->count;
    if (fixture->cancelAfterMatch) umi_cancellation_token_request(fixture->token);
    if (fixture->changeAfterMatch) (void)umi_file_index_clear(fixture->index);
    return UMI_STATUS_OK;
}
static void CancelProgress(const UmiSearchStats *stats, void *userData)
{
    (void)stats; umi_cancellation_token_request(userData);
}
static int WriteAndIndex(Fixture *fixture, const char *path, const void *data, size_t size)
{
    CHECK(umi_fs_write_bytes(path, data, size) == UMI_STATUS_OK);
    CHECK(umi_file_index_rebuild(fixture->index) == UMI_STATUS_OK);
    fixture->count = 0U;
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    Fixture f = {0};
    char cwd[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    UmiSearchStats stats;
    UmiSearchRequest request = umi_search_request_default("note");
    UmiSearchOptions options = {0};
    const char *kind = argc == 2 ? argv[1] : "basic";
    const char *text = "Umicom Notes\n  note note\n";
    CHECK(umi_fs_current_directory(cwd, sizeof cwd) == UMI_STATUS_OK);
    CHECK(umi_path_join(cwd, "file-search-fixture", root, sizeof root) == UMI_STATUS_OK);
    CHECK(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK);
    CHECK(umi_path_join(root, "notes.c", path, sizeof path) == UMI_STATUS_OK);
    UmiFileIndexConfig config = umi_file_index_config_default(root);
    CHECK(umi_file_index_create(&config, &f.index) == UMI_STATUS_OK);
    CHECK(umi_cancellation_token_create(&f.token) == UMI_STATUS_OK);
    CHECK(WriteAndIndex(&f, path, text, strlen(text)) == EXIT_SUCCESS);
    if (strcmp(kind, "basic") == 0) {
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_OK);
        CHECK(stats.matches == 3U && f.matches[1].line == 2U && f.matches[1].column == 3U);
    } else if (strcmp(kind, "case") == 0) {
        request.case_sensitive = 1;
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_OK);
        CHECK(stats.matches == 2U);
    } else if (strcmp(kind, "utf8") == 0 || strcmp(kind, "bom") == 0 || strcmp(kind, "crlf") == 0) {
        const char *sample = strcmp(kind, "utf8") == 0 ? "caf\xc3\xa9 note\n" :
            strcmp(kind, "bom") == 0 ? "\xef\xbb\xbfnote\n" : "first\r\n  note\r\n";
        CHECK(WriteAndIndex(&f, path, sample, strlen(sample)) == EXIT_SUCCESS);
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_OK);
        CHECK(stats.matches == 1U);
        CHECK(f.matches[0].column == (strcmp(kind, "utf8") == 0 ? 7U : strcmp(kind, "bom") == 0 ? 1U : 3U));
        CHECK(f.matches[0].line == (strcmp(kind, "crlf") == 0 ? 2U : 1U));
    } else if (strcmp(kind, "binary") == 0) {
        char *bytes = malloc(8192U); CHECK(bytes != NULL); memset(bytes, 'a', 8192U);
        memcpy(bytes, "note", 4U); bytes[5000] = 0;
        CHECK(WriteAndIndex(&f, path, bytes, 8192U) == EXIT_SUCCESS); free(bytes);
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_OK);
        CHECK(stats.matches == 0U && stats.binary_files_skipped == 1U);
    } else if (strcmp(kind, "grown-file") == 0) {
        request.maximum_file_size = 32U;
        char large[128]; memset(large, 'a', sizeof large); memcpy(large, "note", 4U);
        CHECK(umi_fs_write_bytes(path, large, sizeof large) == UMI_STATUS_OK);
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_OK);
        CHECK(stats.oversized_files_skipped == 1U && stats.matches == 0U);
    } else if (strcmp(kind, "limit-zero") == 0) {
        request.maximum_results = 0U;
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f.count == 0U);
        request.maximum_results = 10U; request.maximum_file_size = 0U;
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(kind, "result-cap") == 0) {
        request.maximum_results = 2U;
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_OK);
        CHECK(stats.matches == 2U && stats.truncated);
        request.maximum_results = 3U; f.count = 0U;
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_OK);
        CHECK(stats.matches == 3U && !stats.truncated);
    } else if (strcmp(kind, "unreadable") == 0) {
        CHECK(remove(path) == 0);
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_IO_ERROR);
    } else if (strcmp(kind, "cancel-before") == 0 || strcmp(kind, "cancel-empty") == 0) {
        if (strcmp(kind, "cancel-empty") == 0) CHECK(umi_file_index_clear(f.index) == UMI_STATUS_OK);
        request.cancellation = f.token; umi_cancellation_token_request(f.token);
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_CANCELLED);
        CHECK(stats.cancelled && stats.matches == 0U);
    } else if (strcmp(kind, "cancel-during") == 0) {
        request.cancellation = f.token; f.cancelAfterMatch = 1;
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_CANCELLED);
        CHECK(stats.cancelled);
    } else if (strcmp(kind, "cancel-progress") == 0) {
        request.cancellation = f.token; options.progress = CancelProgress; options.progressUserData = f.token;
        CHECK(UmiSearchFileIndexWithOptions(f.index, &request, &options, Match, &f, &stats) == UMI_STATUS_CANCELLED);
        CHECK(stats.matches == 0U);
    } else if (strcmp(kind, "sink-error") == 0) {
        f.failSink = 1;
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_IO_ERROR);
        CHECK(stats.matches == 0U);
    } else if (strcmp(kind, "changed-index") == 0) {
        f.changeAfterMatch = 1;
        CHECK(umi_search_file_index(f.index, &request, Match, &f, &stats) == UMI_STATUS_BUSY);
    } else if (strcmp(kind, "stale-revision") == 0) {
        options.expectedRevision = umi_file_index_stats(f.index).revision;
        CHECK(umi_file_index_clear(f.index) == UMI_STATUS_OK);
        CHECK(UmiSearchFileIndexWithOptions(f.index, &request, &options, Match, &f, &stats) == UMI_STATUS_BUSY);
        CHECK(f.count == 0U);
    } else if (strcmp(kind, "locate") == 0) {
        UmiSearchMatch match = {0}; size_t offset = 99U;
        match.line = 2U; match.column = 3U;
        CHECK(UmiSearchMatchLocate(&match, "note", 1, text, strlen(text), &offset) == UMI_STATUS_OK);
        CHECK(offset == 15U);
        const char *changed = "Umicom Notes\n  changed\n"; offset = 99U;
        CHECK(UmiSearchMatchLocate(&match, "note", 1, changed, strlen(changed), &offset) == UMI_STATUS_NOT_FOUND);
        CHECK(offset == 99U); match.column = SIZE_MAX;
        CHECK(UmiSearchMatchLocate(&match, "note", 1, text, strlen(text), &offset) == UMI_STATUS_NOT_FOUND);
    } else if (strncmp(kind, "session-", 8U) == 0) {
        UmiFileSearchSession *session = NULL;
        UmiFileSearchSnapshot state, previous;
        UmiSearchMatch match;
        char query[64] = "note";
        CHECK(UmiFileSearchCreate(f.index, &session) == UMI_STATUS_OK);
        CHECK(UmiFileSearchStart(session, query, 0, 0U) == UMI_STATUS_OK);
        strcpy(query, "changed caller storage");
        if (strcmp(kind, "session-cancel") == 0) CHECK(UmiFileSearchCancel(session) == UMI_STATUS_OK);
        if (strcmp(kind, "session-invalidate") == 0) UmiFileSearchInvalidate(session);
        if (strcmp(kind, "session-destroy") == 0) {
            UmiFileSearchDestroy(session); session = NULL;
        } else {
            CHECK(UmiFileSearchWait(session, 10000U) == UMI_STATUS_OK);
            CHECK(UmiFileSearchRead(session, &state) == UMI_STATUS_OK);
            CHECK(!state.active && strcmp(state.query, "note") == 0);
            if (strcmp(kind, "session-invalidate") == 0) CHECK(state.stale && !state.ready);
            else if (strcmp(kind, "session-cancel") == 0) CHECK(state.status == UMI_STATUS_CANCELLED || state.ready);
            else {
                CHECK(state.ready && state.stats.matches == 3U);
                CHECK(UmiFileSearchMatchAt(session, state.requestId, 1U, &match) == UMI_STATUS_OK);
                CHECK(match.line == 2U && match.column == 3U);
                CHECK(UmiFileSearchMatchAt(session, state.requestId + 1U, 0U, &match) == UMI_STATUS_BUSY);
                CHECK(UmiFileSearchMatchAt(session, state.requestId, 3U, &match) == UMI_STATUS_NOT_FOUND);
                if (strcmp(kind, "session-invalid-input") == 0) {
                    previous = state;
                    CHECK(UmiFileSearchStart(session, "", 0, 0U) == UMI_STATUS_INVALID_ARGUMENT);
                    CHECK(UmiFileSearchStart(session, "a\nb", 0, 0U) == UMI_STATUS_INVALID_ARGUMENT);
                    CHECK(UmiFileSearchRead(session, &state) == UMI_STATUS_OK);
                    CHECK(state.requestId == previous.requestId && state.ready);
                } else if (strcmp(kind, "session-stale") == 0) {
                    CHECK(umi_file_index_clear(f.index) == UMI_STATUS_OK);
                    CHECK(UmiFileSearchMatchAt(session, state.requestId, 0U, &match) == UMI_STATUS_BUSY);
                    CHECK(UmiFileSearchRead(session, &state) == UMI_STATUS_OK && state.stale);
                } else if (strcmp(kind, "session-repeat") == 0) {
                    uint64_t first = state.requestId;
                    for (unsigned i = 0U; i < 24U; ++i) {
                        CHECK(UmiFileSearchStart(session, "Umicom", 1, 0U) == UMI_STATUS_OK);
                        CHECK(UmiFileSearchWait(session, 10000U) == UMI_STATUS_OK);
                    }
                    CHECK(UmiFileSearchRead(session, &state) == UMI_STATUS_OK);
                    CHECK(state.requestId == first + 24U && state.ready && state.stats.matches == 1U);
                }
            }
            UmiFileSearchDestroy(session);
        }
    } else { fprintf(stderr, "Unknown case: %s\n", kind); return EXIT_FAILURE; }
    umi_cancellation_token_destroy(f.token); umi_file_index_destroy(f.index);
    CHECK(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    printf("PASS %s\n", kind);
    return EXIT_SUCCESS;
}
