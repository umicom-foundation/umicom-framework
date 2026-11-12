/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/source_contracts/test_guide_branding.c
 *
 * PURPOSE:
 *   Reject stale artwork and inactive branding while preserving caller source bytes.
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
#include <stdlib.h>
#include "guide_branding_fixture.h"

static void Replace(char *text, size_t capacity, const char *old, const char *next)
{
    char *at = strstr(text, old);
    assert(at != NULL);
    size_t prefix = (size_t)(at - text), tail = strlen(at + strlen(old));
    assert(prefix + strlen(next) + tail < capacity);
    memmove(at + strlen(next), at + strlen(old), tail + 1U);
    memcpy(at, next, strlen(next));
}
static void Wrap(char *text, size_t capacity, const char *begin, const char *end)
{
    size_t n = strlen(text), a = strlen(begin), b = strlen(end);
    assert(n + a + b < capacity);
    memmove(text + a, text, n + 1U);
    memcpy(text, begin, a); memcpy(text + a + n, end, b + 1U);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    char text[8192], original[8192];
    int written = GbTestDocument(text, sizeof(text), strcmp(argv[1], "light") == 0);
    assert(written > 0 && (size_t)written < sizeof(text));
    UmiSourceBrandAssets assets = GbTestAssets();
    UmiSourceContractReport report = {0};
    int expected = 0;
    const char *which = argv[1];
    if (strcmp(which, "dark") == 0 || strcmp(which, "light") == 0) {
    } else if (strcmp(which, "symbol") == 0) {
        Replace(text, sizeof(text), "</body>",
            "<img alt=\"Umicom icon\" data-umicom-guide-symbol=\"66a6757151f8ee55db127716c7e3dce0be8074b64e20eda542e5c1e46ca9c41e\" src=\"data:image/svg+xml;base64,BwgJ\"></body>");
    } else if (strcmp(which, "comment") == 0) {
        Replace(text, sizeof(text), "</body>", "<!-- <img alt=\"Umicom\" src=\"wrong.png\"> --></body>");
    } else if (strcmp(which, "template") == 0) {
        Replace(text, sizeof(text), "</body>", "<template><img alt=\"Umicom\" src=\"wrong.png\"></template></body>");
    } else if (strcmp(which, "nested-template") == 0) {
        Replace(text, sizeof(text), "</body>", "<template><template><img alt=\"Umicom\" src=\"wrong.png\"></template></template></body>");
    } else if (strcmp(which, "script") == 0) {
        Replace(text, sizeof(text), "</body>", "<script>const example = '<img alt=\"Umicom\" src=\"wrong.png\">';</script></body>");
    } else if (strcmp(which, "textarea") == 0) {
        Replace(text, sizeof(text), "</body>", "<textarea><img alt=\"Umicom\" src=\"wrong.png\"></textarea></body>");
    } else if (strcmp(which, "quoted-delimiter") == 0) {
        Replace(text, sizeof(text), "<main>", "<main data-example=\"a > b\">");
    } else if (strcmp(which, "single-quotes") == 0) {
        Replace(text, sizeof(text), "alt=\"Umicom\"", "alt='Umicom'");
        Replace(text, sizeof(text), "name=\"umicom-guide-branding\"", "name='umicom-guide-branding'");
    } else if (strcmp(which, "uppercase") == 0) {
        Replace(text, sizeof(text), "<img alt=", "<IMG ALT=");
        Replace(text, sizeof(text), "<style id=", "<STYLE ID=");
        Replace(text, sizeof(text), "</style>", "</STYLE>");
    } else if (strcmp(which, "missing-meta") == 0) {
        Replace(text, sizeof(text), "umicom-guide-branding", "unrelated"); expected = 1;
    } else if (strcmp(which, "wrong-meta") == 0) {
        Replace(text, sizeof(text), "official-assets", "unknown"); expected = 1;
    } else if (strcmp(which, "duplicate-meta") == 0) {
        Replace(text, sizeof(text), "</head>", "<meta name=\"umicom-guide-branding\" content=\"official-assets\"></head>"); expected = 1;
    } else if (strcmp(which, "missing-logo") == 0) {
        Replace(text, sizeof(text), "data-umicom-guide-logo", "data-unrelated-logo"); expected = 1;
    } else if (strcmp(which, "wrong-hash") == 0) {
        Replace(text, sizeof(text), "787c798e39a5bc1910355bae6d0cd87a36b2e10fd0202a83e3bb6b005da83472", "039058c6f2c0cb492c533b0a4d14ef77cc0f78abccced5287d84a1a2011cfb81"); expected = 1;
    } else if (strcmp(which, "wrong-bytes") == 0) {
        Replace(text, sizeof(text), "base64,BAUG", "base64,BAUH"); expected = 1;
    } else if (strcmp(which, "invalid-base64") == 0) {
        Replace(text, sizeof(text), "base64,BAUG", "base64,BAU!"); expected = 1;
    } else if (strcmp(which, "short-asset") == 0) {
        Replace(text, sizeof(text), "base64,BAUG", "base64,BAU="); expected = 1;
    } else if (strcmp(which, "long-asset") == 0) {
        Replace(text, sizeof(text), "base64,BAUG", "base64,BAUGBAUG"); expected = 1;
    } else if (strcmp(which, "external-asset") == 0) {
        Replace(text, sizeof(text), "data:image/svg+xml;base64,BAUG", "https://example.invalid/logo.svg"); expected = 1;
    } else if (strcmp(which, "raster-asset") == 0) {
        Replace(text, sizeof(text), "data:image/svg+xml", "data:image/png"); expected = 1;
    } else if (strcmp(which, "missing-alt") == 0) {
        Replace(text, sizeof(text), "alt=\"Umicom\"", "title=\"Umicom\""); expected = 1;
    } else if (strcmp(which, "unowned-image") == 0) {
        Replace(text, sizeof(text), "</body>", "<img alt=\"Umicom\" src=\"wrong.png\"></body>"); expected = 1;
    } else if (strcmp(which, "missing-icon") == 0) {
        Replace(text, sizeof(text), "rel=\"icon\"", "rel=\"unrelated\""); expected = 1;
    } else if (strcmp(which, "wrong-icon-type") == 0) {
        Replace(text, sizeof(text), "type=\"image/x-icon\"", "type=\"image/png\""); expected = 1;
    } else if (strcmp(which, "icon-bytes") == 0) {
        Replace(text, sizeof(text), "base64,AAAB", "base64,AAAC"); expected = 1;
    } else if (strcmp(which, "missing-fonts") == 0) {
        Replace(text, sizeof(text), "id=\"umicom-guide-fonts\"", "id=\"unrelated\""); expected = 1;
    } else if (strcmp(which, "font-comment") == 0) {
        Replace(text, sizeof(text), "body{font-family:", "/*body{font-family:");
        Replace(text, sizeof(text), "</style>", "*/</style>"); expected = 1;
    } else if (strcmp(which, "font-role") == 0) {
        Replace(text, sizeof(text), "Cascadia Code", "Unapproved Font"); expected = 1;
    } else if (strcmp(which, "unknown-surface") == 0) {
        Replace(text, sizeof(text), "surface=\"dark\"", "surface=\"unknown\""); expected = 1;
    } else if (strcmp(which, "only-comment") == 0) {
        Wrap(text, sizeof(text), "<!--", "-->"); expected = 1;
    } else if (strcmp(which, "only-template") == 0) {
        Wrap(text, sizeof(text), "<template>", "</template>"); expected = 1;
    } else if (strcmp(which, "unclosed-template") == 0) {
        Wrap(text, sizeof(text), "<template>", ""); expected = 2;
    } else if (strcmp(which, "unmatched-template") == 0) {
        Replace(text, sizeof(text), "</body>", "</template></body>"); expected = 2;
    } else if (strcmp(which, "unclosed-comment") == 0) {
        Replace(text, sizeof(text), "</body>", "<!-- </body>"); expected = 2;
    } else if (strcmp(which, "unclosed-style") == 0) {
        Replace(text, sizeof(text), "</style>", ""); expected = 2;
    } else if (strcmp(which, "duplicate-attribute") == 0) {
        Replace(text, sizeof(text), "alt=\"Umicom\"", "alt=\"Umicom\" ALT=\"another\""); expected = 2;
    } else if (strcmp(which, "unclosed-quote") == 0) {
        Replace(text, sizeof(text), "</body>", "<img alt=\"unterminated></body>"); expected = 2;
    } else if (strcmp(which, "empty-asset") == 0) {
        assets.ico.length = 0U; expected = 2;
    } else if (strcmp(which, "oversized-asset") == 0) {
        assets.ico.length = 2U * 1024U * 1024U + 1U; expected = 2;
    } else if (strcmp(which, "null-inputs") == 0) {
        assert(UmiSourceContractGuideBranding(NULL, 0U, "fixture", &assets, &report) == 2);
        assert(UmiSourceContractGuideBranding(text, strlen(text), NULL, &assets, &report) == 2);
        assert(UmiSourceContractGuideBranding(text, strlen(text), "fixture", NULL, &report) == 2);
        assert(UmiSourceContractGuideBranding(text, strlen(text), "fixture", &assets, NULL) == 2);
        assert(report.files_checked == 0U); return 0;
    } else if (strcmp(which, "embedded-nul") != 0) return 2;
    size_t length = strlen(text);
    if (strcmp(which, "embedded-nul") == 0) { text[length / 2U] = 0; expected = 2; }
    memcpy(original, text, length + 1U);
    int result = UmiSourceContractGuideBranding(text, length, "fixture.html", &assets, &report);
    assert(result == expected);
    assert(memcmp(original, text, length + 1U) == 0);
    if (expected == 0) assert(report.findings == 0U && report.input_errors == 0U && report.files_checked == 1U);
    if (expected == 1) assert(report.findings > 0U && report.input_errors == 0U);
    if (expected == 2) assert(report.input_errors > 0U);
    return 0;
}
