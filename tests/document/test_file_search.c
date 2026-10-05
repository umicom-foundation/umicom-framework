/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_file_search.c
 * PURPOSE: Verify saved document decoding, bounded refusal and editor coordinates through real indexed files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../platform/search_reader_fixture.h"
#include "umicom/document/file_search.h"
#include "umicom/document/text_encoding.h"
typedef struct Matches
{
    SearchFixture *files;
    const char *mode;
    UmiSearchMatch match;
    size_t count;
} Matches;
static UmiStatus Collect(const UmiSearchMatch *match, void *context)
{
    Matches *m = context;
    m->match = *match;
    ++m->count;
    if (strcmp(m->mode, "changed-index") == 0)
        (void)umi_file_index_clear(m->files->index);
    return strcmp(m->mode, "sink-error") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
/* Construct ASCII UTF-16 fixtures directly. This keeps the expected bytes
 * independent from the document decoder being exercised. */
static size_t Utf16(const char *ascii, int big, unsigned char *out)
{
    out[0] = big ? 0xfeU : 0xffU;
    out[1] = big ? 0xffU : 0xfeU;
    size_t n = 2U;
    for (size_t i = 0U; ascii[i] != '\0'; ++i)
    {
        out[n++] = big ? 0U : (unsigned char)ascii[i];
        out[n++] = big ? (unsigned char)ascii[i] : 0U;
    }
    return n;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *cases[] = {"utf8",           "utf8-bom",       "utf16-le",
                                            "utf16-be",       "crlf",           "cr",
                                            "mixed",          "empty",          "bom-only",
                                            "literal-bom",    "unicode",        "scalar-preview",
                                            "invalid-utf8",   "truncated-utf8", "odd-utf16",
                                            "lone-high",      "lone-low",       "invalid-pair",
                                            "binary",         "decoded-null",   "decoded-limit",
                                            "encoded-limit",  "filter",         "case-fold",
                                            "case-sensitive", "cancel",         "stale-index",
                                            "changed-index",  "sink-error",     "session",
                                            "session-filter", "session-stale",  "locate"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    SearchFixture f = {0};
    CHECK(SearchFixtureOpen(&f, "document-search", mode) == 0);
    unsigned char bytes[512];
    const char *source = "top\n  needle\n", *visible = source;
    size_t length = strlen(source), line = 2U, column = 3U;
    memcpy(bytes, source, length);
    if (strcmp(mode, "crlf") == 0 || strcmp(mode, "mixed") == 0)
    {
        source = strcmp(mode, "mixed") == 0 ? "top\r\n  needle\r" : "top\r\n  needle\r\n";
        length = strlen(source);
        memcpy(bytes, source, length);
    }
    if (strcmp(mode, "cr") == 0)
    {
        source = "top\r  needle\r";
        length = strlen(source);
        memcpy(bytes, source, length);
    }
    if (strcmp(mode, "utf8-bom") == 0)
    {
        memcpy(bytes, "\xef\xbb\xbf", 3U);
        memcpy(bytes + 3U, source, length);
        length += 3U;
    }
    if (strcmp(mode, "utf16-le") == 0 || strcmp(mode, "utf16-be") == 0 || strncmp(mode, "session", 7U) == 0 ||
        strcmp(mode, "locate") == 0)
        length = Utf16("top\r\n  needle\r\n", strcmp(mode, "utf16-be") == 0, bytes);
    if (strcmp(mode, "empty") == 0)
        length = 0U;
    if (strcmp(mode, "bom-only") == 0)
    {
        memcpy(bytes, "\xff\xfe", 2U);
        length = 2U;
    }
    if (strcmp(mode, "literal-bom") == 0)
    {
        source = "\xef\xbb\xbf\xef\xbb\xbfneedle";
        length = strlen(source);
        memcpy(bytes, source, length);
        visible = source + 3U;
        line = 1U;
        column = 4U;
    }
    if (strcmp(mode, "unicode") == 0)
    {
        const unsigned char encoded[] = {0xff, 0xfe, 0xea, 0x96, 0x3d, 0xd8, 0x00, 0xde, 0x20, 0x00, 0x6e,
                                         0x00, 0x65, 0x00, 0x65, 0x00, 0x64, 0x00, 0x6c, 0x00, 0x65, 0x00};
        memcpy(bytes, encoded, sizeof(encoded));
        length = sizeof(encoded);
        visible = "\xe9\x9b\xaa\xf0\x9f\x98\x80 needle";
        line = 1U;
        column = 9U;
    }
    if (strcmp(mode, "scalar-preview") == 0)
    {
        memset(bytes, 'x', sizeof(bytes));
        memcpy(bytes, "needle", 6U);
        memcpy(bytes + 317U, "\xe9\x9b\xaa", 3U);
        bytes[340] = '\n';
        length = 341U;
        line = column = 1U;
    }
    int skipped = strcmp(mode, "invalid-utf8") == 0 || strcmp(mode, "truncated-utf8") == 0 ||
                  strcmp(mode, "odd-utf16") == 0 || strcmp(mode, "lone-high") == 0 ||
                  strcmp(mode, "lone-low") == 0 || strcmp(mode, "invalid-pair") == 0 ||
                  strcmp(mode, "binary") == 0 || strcmp(mode, "decoded-null") == 0;
    if (strcmp(mode, "invalid-utf8") == 0)
        bytes[0] = 0xffU;
    if (strcmp(mode, "truncated-utf8") == 0)
    {
        bytes[0] = 0xe9U;
        length = 1U;
    }
    if (strcmp(mode, "odd-utf16") == 0)
    {
        memcpy(bytes, "\xff\xfeX", 3U);
        length = 3U;
    }
    if (strcmp(mode, "lone-high") == 0)
    {
        const unsigned char v[] = {0xff, 0xfe, 0x00, 0xd8};
        memcpy(bytes, v, sizeof(v));
        length = sizeof(v);
    }
    if (strcmp(mode, "lone-low") == 0)
    {
        const unsigned char v[] = {0xfe, 0xff, 0xdc, 0x00};
        memcpy(bytes, v, sizeof(v));
        length = sizeof(v);
    }
    if (strcmp(mode, "invalid-pair") == 0)
    {
        const unsigned char v[] = {0xff, 0xfe, 0x00, 0xd8, 0x41, 0x00};
        memcpy(bytes, v, sizeof(v));
        length = sizeof(v);
    }
    if (strcmp(mode, "binary") == 0)
        bytes[0] = 0U;
    if (strcmp(mode, "decoded-null") == 0)
    {
        const unsigned char v[] = {0xff, 0xfe, 0, 0, 0x41, 0};
        memcpy(bytes, v, sizeof(v));
        length = sizeof(v);
    }
    if (strcmp(mode, "encoded-limit") == 0)
    {
        memset(bytes, 'x', 36U);
        length = 36U;
    }
    CHECK(SearchFixturePut(&f, "note.txt", bytes, length) == 0);
    CHECK(SearchFixtureIndex(&f) == 0);
    UmiSearchRequest request = umi_search_request_default(
        strcmp(mode, "case-sensitive") == 0 || strcmp(mode, "case-fold") == 0 ? "NEEDLE" : "needle");
    request.case_sensitive = strcmp(mode, "case-sensitive") == 0;
    request.cancellation = f.cancel;
    if (strcmp(mode, "decoded-limit") == 0 || strcmp(mode, "encoded-limit") == 0)
        request.maximum_file_size = 8U;
    if (strcmp(mode, "cancel") == 0)
        (void)umi_cancellation_token_request(f.cancel);
    UmiSearchOptions options = {0};
    if (strcmp(mode, "stale-index") == 0)
        options.expectedRevision = umi_file_index_stats(f.index).revision + 1U;
    UmiSearchPathFilter filter;
    CHECK(UmiSearchPathFilterInit(strcmp(mode, "filter") == 0 || strcmp(mode, "session-filter") == 0 ? "*.c"
                                                                                                     : "",
                                  "", &filter) == UMI_STATUS_OK);
    /* Name the fixture inputs and let C zero-initialise the remaining match
     * record and count. This avoids assuming how many aggregate levels the
     * shared match record contains. Retain the positional spelling for review. */
#if 0
    Matches matches = {&f, mode, {0}, 0U};
#endif
    /* Name the fixture inputs and let C zero-initialise the remaining match
     * record and count. */
    Matches matches = {.files = &f, .mode = mode};
    UmiSearchStats stats;
    UmiStatus status;
    if (strncmp(mode, "session", 7U) == 0)
    {
        CHECK(UmiDocumentFileSearchCreate(f.index, &f.session) == UMI_STATUS_OK);
        CHECK(UmiFileSearchStartFiltered(f.session, "needle", 1, 0U, &filter) == UMI_STATUS_OK);
        CHECK(UmiFileSearchWait(f.session, 10000U) == UMI_STATUS_OK);
        UmiFileSearchSnapshot state;
        CHECK(UmiFileSearchRead(f.session, &state) == UMI_STATUS_OK);
        CHECK(state.ready);
        status = state.status;
        stats = state.stats;
        if (stats.matches != 0U)
        {
            CHECK(UmiFileSearchMatchAt(f.session, state.requestId, 0U, &matches.match) == UMI_STATUS_OK);
            matches.count = 1U;
        }
        if (strcmp(mode, "session-stale") == 0)
        {
            CHECK(umi_file_index_clear(f.index) == UMI_STATUS_OK);
            CHECK(UmiFileSearchRead(f.session, &state) == UMI_STATUS_OK && state.stale && !state.ready);
            CHECK(UmiFileSearchMatchAt(f.session, state.requestId, 0U, &matches.match) == UMI_STATUS_BUSY);
        }
    }
    else
        status = UmiDocumentSearchFileIndex(f.index, &request, &options, &filter, Collect, &matches, &stats);
    UmiStatus wanted = strcmp(mode, "cancel") == 0 ? UMI_STATUS_CANCELLED
                       : strcmp(mode, "stale-index") == 0 || strcmp(mode, "changed-index") == 0
                           ? UMI_STATUS_BUSY
                       : strcmp(mode, "sink-error") == 0 ? UMI_STATUS_IO_ERROR
                                                         : UMI_STATUS_OK;
    CHECK(status == wanted);
    int no_match = skipped || strcmp(mode, "empty") == 0 || strcmp(mode, "bom-only") == 0 ||
                   strcmp(mode, "filter") == 0 || strcmp(mode, "session-filter") == 0 ||
                   strcmp(mode, "case-sensitive") == 0 || strcmp(mode, "decoded-limit") == 0 ||
                   strcmp(mode, "encoded-limit") == 0;
    if (status == UMI_STATUS_OK)
    {
        CHECK(stats.matches == (no_match ? 0U : 1U));
        CHECK(matches.count == (no_match ? 0U : 1U));
        if (!no_match)
            CHECK(matches.match.line == line && matches.match.column == column);
        if (skipped)
            CHECK(stats.binary_files_skipped == 1U && stats.files_searched == 0U);
        if (strcmp(mode, "decoded-limit") == 0 || strcmp(mode, "encoded-limit") == 0)
            CHECK(stats.oversized_files_skipped == 1U);
        if (strcmp(mode, "scalar-preview") == 0)
        {
            CHECK(strlen(matches.match.preview) == 317U);
            CHECK(umi_document_utf8_validate((const unsigned char *)matches.match.preview,
                                             strlen(matches.match.preview), NULL));
        }
        else if (!no_match)
        {
            size_t offset = 999U;
            CHECK(UmiSearchMatchLocate(&matches.match, "needle", 1, visible, strlen(visible), &offset) ==
                  UMI_STATUS_OK);
            CHECK(memcmp(visible + offset, "needle", 6U) == 0);
        }
    }
    SearchFixtureStop(&f);
    return 0;
}
