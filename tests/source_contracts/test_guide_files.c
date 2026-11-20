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

static void CheckBundled(const char *which, const char *docs, const char *masters)
{
    /* Generate an owned asset tree independent of the reference assets. An
     * altered file or link must report a finding even if its name is valid. */
    char *assets = ScJoin(docs, "assets");
    assert(assets != NULL); Directory(assets);
    if (strcmp(which, "missing-ico") != 0) WriteAt(assets, "umicom.ico", GbTestIco, 3U);
    WriteAt(assets, "umicom-logo.svg", strcmp(which, "altered-logo") == 0 ? GbTestDark : GbTestLight, 3U);
    WriteAt(assets, "umicom-logo-on-dark.svg", GbTestDark, 3U);
    WriteAt(assets, "umicom-icon.svg", GbTestSymbol, 3U);
    const char *stylesheet =
        "body{font-family: \"Segoe UI Variable Text\", \"Segoe UI\", sans-serif;}"
        "code{font-family: \"Cascadia Code\", Consolas, monospace;}";
    if (strcmp(which, "fonts-in-comment") == 0)
        stylesheet = "/* font-family: \"Segoe UI Variable Text\", \"Segoe UI\", sans-serif;"
                     "font-family: \"Cascadia Code\", Consolas, monospace; */";
    WriteAt(assets, "umicom-documentation.css", stylesheet, strlen(stylesheet));

    /* Test depth-relative links, inert examples and coexistence with a
     * self-contained authored page in the same source distribution. */
    bool nested = strcmp(which, "nested") == 0;
    bool mixed = strcmp(which, "mixed") == 0;
    bool mixed_page = strncmp(which, "mixed-page", 10U) == 0;
    const char *prefix = nested ? "../" : "";
    const char *logo_prefix = strcmp(which, "wrong-depth") == 0 ? "../" : prefix;
    const char *leaf = strcmp(which, "dark") == 0 ? "umicom-logo-on-dark.svg" : "umicom-logo.svg";
    char html[4096];
    int length = snprintf(html, sizeof(html),
        "<!doctype html><html><head>%s<link rel='stylesheet' href='%sassets/umicom-documentation.css'>"
        "<link rel='icon' type='image/svg+xml' href='%sassets/umicom-icon.svg'>"
        "<link rel='icon' type='image/x-icon' href='%sassets/umicom.ico'></head><body>%s"
        "<img class='umicom-doc-logo' src='%sassets/%s' alt='%s'></body></html>",
        strcmp(which, "changed-base") == 0 ? "<base href='alternate/'>" : "", prefix, prefix, prefix,
        strcmp(which, "inert-marker") == 0 ?
            "<template><meta name='umicom-guide-branding' content='official-assets'></template>" : "",
        logo_prefix, leaf, strcmp(which, "missing-alt") == 0 ? "" : "Umicom");
    assert(length > 0 && (size_t)length < sizeof(html));
    if (mixed_page) {
        /* Existing packaged guides can retain an approved embedded icon and
         * wordmark. Check those bytes as well as the linked package files. */
        char embedded[4096];
        int embedded_length = GbTestDocument(embedded, sizeof(embedded), false);
        assert(embedded_length > 0 && (size_t)embedded_length < sizeof(embedded));
        char *end = strstr(embedded, "</head>");
        assert(end != NULL);
        const char *links = "<link rel='stylesheet' href='assets/umicom-documentation.css'>"
            "<link rel='icon' type='image/svg+xml' href='assets/umicom-icon.svg'>"
            "<link rel='icon' type='image/x-icon' href='assets/umicom.ico'>";
        if (strcmp(which, "mixed-page-wrong-ico") == 0) {
            const char *data_prefix = "data:image/x-icon;base64,";
            char *data = strstr(embedded, data_prefix);
            assert(data != NULL);
            data += strlen(data_prefix);
            *data = *data == 'A' ? 'B' : 'A';
        }
        length = snprintf(html, sizeof(html), "%.*s%s%s", (int)(end - embedded), embedded, links, end);
        assert(length > 0 && (size_t)length < sizeof(html));
    }
    char *nested_root = nested ? ScJoin(docs, "more lessons") : NULL;
    if (nested) { assert(nested_root != NULL); Directory(nested_root); }
    WriteAt(nested ? nested_root : docs, "linked.html", html, (size_t)length);
    if (mixed) {
        length = GbTestDocument(html, sizeof(html), false);
        assert(length > 0 && (size_t)length < sizeof(html));
        WriteAt(docs, "embedded.html", html, (size_t)length);
    }
    bool qualifies = nested || mixed || strcmp(which, "valid") == 0 ||
        strcmp(which, "dark") == 0 || strcmp(which, "inert-marker") == 0 || strcmp(which, "mixed-page") == 0;
    UmiSourceContractReport report = {0};
    const char *roots[] = {docs};
    assert(UmiSourceContractAuditBundledGuides(roots, 1U, masters, &report) == (qualifies ? 0 : 1));
    assert(report.files_checked == (mixed ? 2U : 1U) && report.input_errors == 0U);
    assert((report.findings == 0U) == qualifies);
    free(nested_root);
    free(assets);
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
    if (strncmp(which, "bundle-", 7U) == 0) {
        CheckBundled(which + 7U, docs, assets);
        free(assets); free(docs);
        return 0;
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
