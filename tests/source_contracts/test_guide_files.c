/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/source_contracts/test_guide_files.c
 *
 * PURPOSE:
 *   Exercise bounded binary asset reads, guide discovery and missing-input refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include <assert.h>
#include "internal.h"
#include "guide_branding_fixture.h"
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
static wchar_t *FixtureWide(const char *path)
{
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    assert(count > 0);
    wchar_t *out = malloc((size_t)count * sizeof(*out));
    assert(out != NULL);
    assert(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, out, count) == count);
    return out;
}
#endif
static void Directory(const char *path)
{
#ifdef _WIN32
    wchar_t *wide = FixtureWide(path); int status = _wmkdir(wide); free(wide);
#else
    int status = mkdir(path, 0700);
#endif
    assert(status == 0 || errno == EEXIST);
}
static void Write(const char *path, const void *bytes, size_t length)
{
#ifdef _WIN32
    wchar_t *wide = FixtureWide(path); FILE *file = _wfopen(wide, L"wb"); free(wide);
#else
    FILE *file = fopen(path, "wb");
#endif
    assert(file != NULL);
    assert(fwrite(bytes, 1U, length, file) == length);
    assert(fclose(file) == 0);
}
static void WriteAt(const char *root, const char *name, const void *bytes, size_t length)
{
    char *path = ScJoin(root, name); assert(path != NULL);
    Write(path, bytes, length); free(path);
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *which = argv[1];
    char *assets = ScJoin(argv[2], "assets");
    char *docs = ScJoin(argv[2], "guides");
    assert(assets != NULL && docs != NULL);
    Directory(argv[2]); Directory(assets); Directory(docs);
    const char *names[] = {"umicom.ico", "umicom-logo.svg", "umicom-logo-on-dark.svg", "umicom-icon.svg"};
    const unsigned char *bytes[] = {GbTestIco, GbTestLight, GbTestDark, GbTestSymbol};
    for (size_t i = 0U; i < 4U; ++i) {
        if (i == 0U && strcmp(which, "missing-asset") == 0) continue;
        size_t length = i == 0U && strcmp(which, "empty-asset") == 0 ? 0U : 3U;
        WriteAt(assets, names[i], bytes[i], length);
    }
    char html[4096];
    int written = GbTestDocument(html, sizeof(html), false);
    assert(written > 0 && (size_t)written < sizeof(html));
    int expected = 0;
    size_t expected_files = 1U;
    const char *roots[] = {docs, docs};
    size_t count = 1U;
    char *nested = NULL;
    if (strcmp(which, "empty") == 0 || strcmp(which, "unrelated-file") == 0 ||
        strcmp(which, "ignored-directory") == 0) {
        expected = 2; expected_files = 0U;
        if (strcmp(which, "unrelated-file") == 0) WriteAt(docs, "notes.txt", html, (size_t)written);
        if (strcmp(which, "ignored-directory") == 0) {
            nested = ScJoin(docs, "build-example"); assert(nested != NULL); Directory(nested);
            WriteAt(nested, "example.html", html, (size_t)written);
        }
    } else {
        WriteAt(docs, "lesson.html", html, (size_t)written);
        if (strcmp(which, "valid") == 0) {
        } else if (strcmp(which, "duplicates") == 0) count = 2U;
        else if (strcmp(which, "missing-root") == 0) {
            nested = ScJoin(docs, "not-created"); assert(nested != NULL);
            roots[0] = nested; expected = 2; expected_files = 0U;
        } else if (strcmp(which, "missing-asset") == 0 || strcmp(which, "empty-asset") == 0) {
            expected = 2; expected_files = 0U;
        } else if (strcmp(which, "binary-html") == 0) {
            html[10] = 0; WriteAt(docs, "lesson.html", html, (size_t)written);
            expected = 2; expected_files = 0U;
        } else if (strcmp(which, "altered-asset") == 0) {
            WriteAt(assets, "umicom-logo-on-dark.svg", GbTestLight, 3U); expected = 1;
        } else if (strcmp(which, "nested") == 0) {
            nested = ScJoin(docs, "more lessons"); assert(nested != NULL); Directory(nested);
            WriteAt(nested, "second.html", html, (size_t)written); expected_files = 2U;
        } else if (strcmp(which, "read-bytes") == 0 || strcmp(which, "read-text-nul") == 0) {
            char *input = ScJoin(assets, "umicom.ico"); assert(input != NULL);
            size_t size = 99U;
            unsigned char *actual = ScReadBytes(input, &size);
            assert(actual != NULL && size == 3U && memcmp(actual, GbTestIco, 3U) == 0 && actual[3] == 0U);
            free(actual);
            assert(ScRead(input) == NULL); free(input);
        } else { free(nested); free(assets); free(docs); return 2; }
    }
    UmiSourceContractReport report = {0};
    assert(UmiSourceContractAuditGuides(roots, count, assets, &report) == expected);
    assert(report.files_checked == expected_files);
    if (expected == 0) assert(report.findings == 0U && report.input_errors == 0U);
    if (expected == 1) assert(report.findings > 0U && report.input_errors == 0U);
    if (expected == 2) assert(report.input_errors > 0U);
    free(nested); free(assets); free(docs);
    return 0;
}
