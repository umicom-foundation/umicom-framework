/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_search_filter.c
 * PURPOSE: Verify bounded patterns and scoped searches over real indexed files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/search_session.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define PROCESS_ID _getpid
#else
#include <unistd.h>
#define PROCESS_ID getpid
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

/* These cases exercise syntax and wildcard behavior without depending on a
 * platform filesystem's case sensitivity or support for wildcard filenames. */
static int Patterns(void)
{
    const struct { const char *include, *exclude, *path; int match; } cases[] = {
        {"", "", "src/notes.c", 1}, {"*.c;*.h", "", "src/notes.c", 1},
        {"*.c;*.h", "", "README.md", 0}, {"src/*", "", "src/deep/notes.c", 1},
        {"src/*", "", "other/src/notes.c", 0}, {"*.c", "generated/*", "generated/auto.c", 0},
        {"", "*.c", "src/notes.h", 1}, {"*.C", "", "src/notes.c", 0},
        {"note?.c", "", "src/notes.c", 1}, {"note?.c", "", "src/note.c", 0},
        {"a*b*c", "", "aaabbbbc", 1}, {"*a", "", "*ba", 1},
        {"**.c", "", "notes.c", 1}, {"*.c", "", "notes.c.extra", 0},
        {"src\\*.c", "", "src\\deep\\notes.c", 1},
        {"*.c", "", "src/caf\xc3\xa9.c", 1}, {"*.c", "", "folder with spaces/my notes.c", 1},
        {"  *.h ; *.c \t", " *.min.c ", "src/helper.min.c", 0}
    };
    UmiSearchPathFilter filter;
    for (size_t index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        int matches = -1;
        CHECK(UmiSearchPathFilterInit(cases[index].include, cases[index].exclude, &filter) == UMI_STATUS_OK);
        CHECK(UmiSearchPathFilterMatch(&filter, cases[index].path, &matches) == UMI_STATUS_OK);
        CHECK(matches == cases[index].match);
    }
    CHECK(UmiSearchPathFilterInit(" *.c ; src\\*.h ", "", &filter) == UMI_STATUS_OK);
    CHECK(strcmp(filter.include_patterns, "*.c;src/*.h") == 0);
    CHECK(UmiSearchPathFilterInit(filter.include_patterns, filter.exclude_patterns, &filter) == UMI_STATUS_OK);
    UmiSearchPathFilter before = filter;
    const char *invalid[] = {"*.c;", ";*.c", "*.c;;*.h", "*.c; \t;*.h", "[ab].c", "{a,b}.c",
        "C:\\src\\*.c", "/src/*", "*.c\n*.h", "*.c\t*.h"};
    for (size_t index = 0U; index < sizeof(invalid) / sizeof(invalid[0]); ++index) {
        CHECK(UmiSearchPathFilterInit(invalid[index], "", &filter) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&filter, &before, sizeof(filter)) == 0);
    }
    char overlong[UMI_SEARCH_FILTER_TEXT_CAPACITY]; memset(overlong, 'a', sizeof(overlong));
    CHECK(UmiSearchPathFilterInit(overlong, "", &filter) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&filter, &before, sizeof(filter)) == 0);
    char lists[64] = "a";
    for (size_t index = 1U; index < UMI_SEARCH_FILTER_MAX_PATTERNS; ++index) strcat(lists, ";a");
    CHECK(UmiSearchPathFilterInit(lists, "", &filter) == UMI_STATUS_OK);
    strcat(lists, ";a");
    CHECK(UmiSearchPathFilterInit(lists, "", &filter) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiSearchPathFilterInit(NULL, "", &filter) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiSearchPathFilterInit("", NULL, &filter) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiSearchPathFilterInit("", "", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    int matches = 77;
    CHECK(UmiSearchPathFilterMatch(NULL, "notes.c", &matches) == UMI_STATUS_INVALID_ARGUMENT && matches == 77);
    CHECK(UmiSearchPathFilterMatch(&filter, "", &matches) == UMI_STATUS_INVALID_ARGUMENT && matches == 77);
    CHECK(UmiSearchPathFilterMatch(&filter, "/notes.c", &matches) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiSearchPathFilterMatch(&filter, "folder/", &matches) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiSearchPathFilterMatch(&filter, "notes.c", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

typedef struct Fixture {
    UmiFileIndex *index;
    UmiFileSearchSession *session;
    UmiCancellationToken *cancel;
    char root[UMI_PATH_CAPACITY];
    char generated[UMI_PATH_CAPACITY];
    char moved[UMI_PATH_CAPACITY];
    size_t matches;
    int made_root;
} Fixture;

static UmiStatus Match(const UmiSearchMatch *match, void *context)
{
    Fixture *f = context;
    if (strstr(match->preview, "FindMarker") == NULL) return UMI_STATUS_INVALID_STATE;
    ++f->matches;
    return UMI_STATUS_OK;
}

static void InvalidateIndex(const UmiSearchStats *stats, void *context)
{
    (void)stats;
    (void)umi_file_index_clear(context);
}

static int Start(Fixture *f, const char *name)
{
    char temp[UMI_PATH_CAPACITY], leaf[128], path[UMI_PATH_CAPACITY];
    CHECK(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    (void)snprintf(leaf, sizeof(leaf), "umicom-search-filter-%ld-%s", (long)PROCESS_ID(), name);
    CHECK(umi_path_join(temp, leaf, f->root, sizeof(f->root)) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(f->root));
    CHECK(umi_fs_make_directories(f->root) == UMI_STATUS_OK); f->made_root = 1;
    const char *folders[] = {"src", "include", "generated"};
    for (size_t index = 0U; index < sizeof(folders) / sizeof(folders[0]); ++index) {
        CHECK(umi_path_join(f->root, folders[index], path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_fs_make_directories(path) == UMI_STATUS_OK);
    }
    const char *files[] = {"notes.c", "src/notes.c", "include/notes.h", "generated/auto.c", "README.md"};
    for (size_t index = 0U; index < sizeof(files) / sizeof(files[0]); ++index) {
        CHECK(umi_path_join(f->root, files[index], path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_fs_write_text(path, "FindMarker\n") == UMI_STATUS_OK);
    }
    CHECK(umi_path_join(f->root, "generated/auto.c", f->generated, sizeof(f->generated)) == UMI_STATUS_OK);
    CHECK(umi_path_join(f->root, "moved.c", f->moved, sizeof(f->moved)) == UMI_STATUS_OK);
    UmiFileIndexConfig config = umi_file_index_config_default(f->root);
    CHECK(umi_file_index_create(&config, &f->index) == UMI_STATUS_OK);
    CHECK(umi_file_index_rebuild(f->index) == UMI_STATUS_OK);
    CHECK(umi_file_index_stats(f->index).files == 5U);
    CHECK(umi_cancellation_token_create(&f->cancel) == UMI_STATUS_OK);
    return 0;
}

static void Stop(Fixture *f)
{
    UmiFileSearchDestroy(f->session);
    umi_cancellation_token_destroy(f->cancel);
    umi_file_index_destroy(f->index);
    if (f->made_root) (void)umi_fs_remove_tree(f->root);
}

static int Run(Fixture *f, const char *name)
{
    UmiSearchRequest request = umi_search_request_default("FindMarker");
    UmiSearchOptions options = {0};
    UmiSearchStats stats = {0};
    UmiSearchPathFilter filter;
    CHECK(UmiSearchPathFilterInit("*.c;*.h", "generated/*", &filter) == UMI_STATUS_OK);
    if (strcmp(name, "session") == 0) {
        UmiFileSearchSnapshot state, previous;
        UmiSearchPathFilter captured;
        UmiSearchMatch match;
        CHECK(UmiFileSearchCreate(f->index, &f->session) == UMI_STATUS_OK);
        CHECK(UmiFileSearchStartFiltered(f->session, "FindMarker", 1, 0U, &filter) == UMI_STATUS_OK);
        /* Inputs may be reused immediately; the worker owns its own copy. */
        CHECK(UmiSearchPathFilterInit("*.md", "", &filter) == UMI_STATUS_OK);
        CHECK(UmiFileSearchWait(f->session, 10000U) == UMI_STATUS_OK);
        CHECK(UmiFileSearchRead(f->session, &state) == UMI_STATUS_OK && state.ready && state.stats.matches == 3U);
        CHECK(UmiFileSearchFilterRead(f->session, state.requestId, &captured) == UMI_STATUS_OK);
        CHECK(strcmp(captured.include_patterns, "*.c;*.h") == 0 && strcmp(captured.exclude_patterns, "generated/*") == 0);
        CHECK(UmiFileSearchMatchAt(f->session, state.requestId, 0U, &match) == UMI_STATUS_OK);
        previous = state;
        memset(filter.include_patterns, 'x', sizeof(filter.include_patterns));
        CHECK(UmiFileSearchStartFiltered(f->session, "FindMarker", 1, 0U, &filter) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiFileSearchRead(f->session, &state) == UMI_STATUS_OK && state.requestId == previous.requestId && state.ready);
        CHECK(UmiFileSearchFilterRead(f->session, state.requestId + 1U, &captured) == UMI_STATUS_BUSY);
        CHECK(strcmp(captured.include_patterns, "*.c;*.h") == 0);
        CHECK(UmiFileSearchStart(f->session, "FindMarker", 1, 0U) == UMI_STATUS_OK);
        CHECK(UmiFileSearchWait(f->session, 10000U) == UMI_STATUS_OK);
        CHECK(UmiFileSearchRead(f->session, &state) == UMI_STATUS_OK && state.ready && state.stats.matches == 5U);
        CHECK(UmiFileSearchFilterRead(f->session, state.requestId, &captured) == UMI_STATUS_OK);
        CHECK(captured.include_patterns[0] == '\0' && captured.exclude_patterns[0] == '\0');
        UmiFileSearchInvalidate(f->session);
        CHECK(UmiFileSearchFilterRead(f->session, state.requestId, &captured) == UMI_STATUS_BUSY);
        return 0;
    }
    if (strcmp(name, "path") == 0) CHECK(UmiSearchPathFilterInit("src/*", "", &filter) == UMI_STATUS_OK);
    if (strcmp(name, "result-limit") == 0) {
        CHECK(UmiSearchPathFilterInit("*.md", "", &filter) == UMI_STATUS_OK); request.maximum_results = 1U;
    }
    if (strcmp(name, "excluded-unreadable") == 0) CHECK(umi_fs_rename(f->generated, f->moved) == UMI_STATUS_OK);
    if (strcmp(name, "cancel") == 0) { request.cancellation = f->cancel; umi_cancellation_token_request(f->cancel); }
    if (strcmp(name, "changed-index") == 0) { options.progress = InvalidateIndex; options.progressUserData = f->index; }
    if (strcmp(name, "invalid") == 0) {
        memset(filter.exclude_patterns, 'x', sizeof(filter.exclude_patterns)); stats.matches = 77U;
        CHECK(UmiSearchFileIndexScoped(f->index, &request, &options, &filter, Match, f, &stats) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(stats.matches == 77U && f->matches == 0U); return 0;
    }
    UmiStatus status = UmiSearchFileIndexScoped(f->index, &request, &options, &filter, Match, f, &stats);
    if (strcmp(name, "cancel") == 0) { CHECK(status == UMI_STATUS_CANCELLED && stats.cancelled && f->matches == 0U); return 0; }
    if (strcmp(name, "changed-index") == 0) { CHECK(status == UMI_STATUS_BUSY && f->matches == 0U); return 0; }
    size_t expected = strcmp(name, "path") == 0 || strcmp(name, "result-limit") == 0 ? 1U : 3U;
    CHECK(status == UMI_STATUS_OK && stats.files_considered == 5U && stats.files_searched == expected);
    CHECK(stats.matches == expected && f->matches == expected && !stats.truncated);
    if (strcmp(name, "excluded-unreadable") == 0) {
        f->matches = 0U;
        CHECK(umi_search_file_index(f->index, &request, Match, f, &stats) == UMI_STATUS_IO_ERROR);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "patterns") == 0) return Patterns();
    const char *cases[] = {"include-exclude", "path", "result-limit", "excluded-unreadable", "cancel", "changed-index", "invalid", "session"};
    int known = 0;
    for (size_t index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index)
        if (strcmp(argv[1], cases[index]) == 0) known = 1;
    if (!known) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    if (fixture == NULL) return 1;
    int status = Start(fixture, argv[1]);
    if (status == 0) status = Run(fixture, argv[1]);
    Stop(fixture); free(fixture); return status;
}
