/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/guide_branding.c
 *
 * PURPOSE:
 *   Check active standalone-guide identity against approved local SVG and ICO bytes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "internal.h"
#include "umicom/base/base64.h"
#include "umicom/base/sha256.h"
#include <stdlib.h>
#include <string.h>

typedef struct GbSpan { const char *text; size_t length; } GbSpan;
typedef struct GbAttribute { GbSpan name, value; } GbAttribute;
typedef struct GbTag {
    GbSpan name;
    GbAttribute attributes[64];
    size_t count, end;
    bool closing, self_closing;
} GbTag;
static unsigned char GbLower(unsigned char c)
{ return c >= 'A' && c <= 'Z' ? (unsigned char)(c + ('a' - 'A')) : c; }
static bool GbSpace(char c)
{ return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f'; }
static bool GbEqual(GbSpan span, const char *value)
{ return span.length == strlen(value) && (span.length == 0U ||
    (span.text != NULL && memcmp(span.text, value, span.length) == 0)); }
static bool GbName(GbSpan span, const char *value)
{
    if (span.length != strlen(value)) return false;
    for (size_t i = 0U; i < span.length; ++i)
        if (GbLower((unsigned char)span.text[i]) != (unsigned char)value[i]) return false;
    return true;
}
static bool GbContains(GbSpan span, const char *value)
{
    size_t n = strlen(value);
    if (n > span.length) return false;
    for (size_t i = 0U; i <= span.length - n; ++i)
        if (memcmp(span.text + i, value, n) == 0) return true;
    return false;
}
static GbSpan GbGet(const GbTag *tag, const char *name)
{
    for (size_t i = 0U; i < tag->count; ++i)
        if (GbName(tag->attributes[i].name, name)) return tag->attributes[i].value;
    return (GbSpan){NULL, 0U};
}
static bool GbHas(const GbTag *tag, const char *name)
{
    for (size_t i = 0U; i < tag->count; ++i)
        if (GbName(tag->attributes[i].name, name)) return true;
    return false;
}
static bool GbSameName(GbSpan a, GbSpan b)
{
    if (a.length != b.length) return false;
    for (size_t i = 0U; i < a.length; ++i)
        if (GbLower((unsigned char)a.text[i]) != GbLower((unsigned char)b.text[i])) return false;
    return true;
}
/* This scanner owns only a narrow source contract. Quoted attribute values may
 * contain >, and inline SVG data is never copied into a fixed attribute array.
 * Keep bounded views into the caller's immutable source instead. */
static int GbReadTag(const char *html, size_t length, size_t start, GbTag *tag)
{
    size_t i = start + 1U;
    memset(tag, 0, sizeof(*tag));
    if (i < length && html[i] == '/') { tag->closing = true; ++i; }
    size_t name = i;
    while (i < length && !GbSpace(html[i]) && html[i] != '/' && html[i] != '>') ++i;
    tag->name = (GbSpan){html + name, i - name};
    if (i == name) return 2;
    for (;;) {
        while (i < length && GbSpace(html[i])) ++i;
        if (i == length) return 2;
        if (html[i] == '>') { tag->end = i + 1U; return 0; }
        if (html[i] == '/' && i + 1U < length && html[i + 1U] == '>') {
            tag->self_closing = true; tag->end = i + 2U; return 0;
        }
        if (tag->closing || tag->count == 64U) return 2;
        size_t begin = i;
        while (i < length && !GbSpace(html[i]) && html[i] != '=' && html[i] != '>' && html[i] != '/') {
            if (html[i] == '\'' || html[i] == '"' || html[i] == '<') return 2;
            ++i;
        }
        if (i == begin) return 2;
        GbAttribute a = {{html + begin, i - begin}, {html + i, 0U}};
        while (i < length && GbSpace(html[i])) ++i;
        if (i < length && html[i] == '=') {
            ++i;
            while (i < length && GbSpace(html[i])) ++i;
            if (i == length) return 2;
            char quote = html[i] == '\'' || html[i] == '"' ? html[i++] : 0;
            begin = i;
            if (quote != 0) {
                while (i < length && html[i] != quote) ++i;
                if (i == length) return 2;
                a.value = (GbSpan){html + begin, i - begin}; ++i;
            } else {
                while (i < length && !GbSpace(html[i]) && html[i] != '>') ++i;
                if (i == begin) return 2;
                a.value = (GbSpan){html + begin, i - begin};
            }
        }
        for (size_t j = 0U; j < tag->count; ++j)
            if (GbSameName(a.name, tag->attributes[j].name)) return 2;
        tag->attributes[tag->count++] = a;
    }
}
static size_t GbFind(const char *text, size_t length, size_t from, const char *needle)
{
    size_t n = strlen(needle);
    if (n > length || from > length - n) return length;
    for (size_t i = from; i <= length - n; ++i)
        if (memcmp(text + i, needle, n) == 0) return i;
    return length;
}
static int GbRawEnd(const char *html, size_t length, size_t from,
    GbSpan name, size_t *content_end, size_t *after)
{
    for (size_t i = from; i < length; ++i) {
        if (html[i] != '<' || i + 1U == length || html[i + 1U] != '/') continue;
        size_t begin = i + 2U;
        if (name.length > length - begin) continue;
        if (!GbSameName((GbSpan){html + begin, name.length}, name)) continue;
        size_t finish = begin + name.length;
        while (finish < length && GbSpace(html[finish])) ++finish;
        if (finish < length && html[finish] == '>') {
            *content_end = i; *after = finish + 1U; return 0;
        }
    }
    return 2;
}
/* Ignore CSS comments when checking the owned font declaration. This remains
 * a declared-font check, not cascade evaluation or font availability proof. */
static bool GbFont(GbSpan css, const char *declaration)
{
    size_t n = strlen(declaration);
    for (size_t i = 0U; i < css.length;) {
        if (i + 1U < css.length && css.text[i] == '/' && css.text[i + 1U] == '*') {
            size_t end = GbFind(css.text, css.length, i + 2U, "*/");
            if (end == css.length) return false;
            i = end + 2U;
        } else {
            if (n <= css.length - i && memcmp(css.text + i, declaration, n) == 0) return true;
            ++i;
        }
    }
    return false;
}
static void GbFinding(UmiSourceContractReport *report, const char *path,
    const char *rule, const char *message)
{ ScFinding(report, path, 1U, rule, message, false); }
static int GbAsset(GbSpan identity, GbSpan uri, const char *prefix,
    UmiSourceBrandAsset expected, const char *path, UmiSourceContractReport *report)
{
    char digest[65];
    ++report->checks;
    if (UmiSha256Buffer(expected.bytes, expected.length, digest) != UMI_STATUS_OK) return 2;
    size_t prefix_length = strlen(prefix), count = 0U;
    if (!GbEqual(identity, digest) || uri.length < prefix_length ||
        memcmp(uri.text, prefix, prefix_length) != 0) {
        GbFinding(report, path, "guide.asset.identity", "Use the approved asset digest and inline SVG/ICO format");
        return 1;
    }
    const char *encoded = uri.text + prefix_length;
    size_t encoded_length = uri.length - prefix_length;
    UmiStatus status = UmiBase64DecodedSize(encoded, encoded_length, 0U, &count);
    if (status != UMI_STATUS_OK || count != expected.length) {
        GbFinding(report, path, "guide.asset.encoding", "Asset encoding or decoded byte length differs from the approved asset");
        return 1;
    }
    unsigned char *bytes = malloc(count);
    if (bytes == NULL) return 2;
    status = UmiBase64Decode(encoded, encoded_length, 0U, bytes, count, &count);
    bool matches = status == UMI_STATUS_OK && memcmp(bytes, expected.bytes, expected.length) == 0;
    free(bytes);
    if (!matches) {
        GbFinding(report, path, "guide.asset.bytes", "Decoded artwork differs from the approved local file");
        return 1;
    }
    return 0;
}
static bool GbAssetValid(UmiSourceBrandAsset asset)
{ return asset.bytes != NULL && asset.length != 0U && asset.length <= 2U * 1024U * 1024U; }
int UmiSourceContractGuideBranding(const char *html, size_t length,
    const char *path, const UmiSourceBrandAssets *assets, UmiSourceContractReport *report)
{
    if (report == NULL || path == NULL || assets == NULL || html == NULL) return 2;
    if (length == 0U || length > SC_MAX_FILE_BYTES || memchr(html, 0, length) != NULL ||
        !GbAssetValid(assets->ico) || !GbAssetValid(assets->logo_light) ||
        !GbAssetValid(assets->logo_dark) || !GbAssetValid(assets->symbol)) {
        ScFinding(report, path, 1U, "guide.input", "Invalid source or canonical asset extent", true); return 2;
    }
    size_t before = report->findings, meta = 0U, logo = 0U, ico = 0U, fonts = 0U, inert = 0U;
    ++report->files_checked;
    for (size_t i = 0U; i < length;) {
        if (html[i] != '<') { ++i; continue; }
        if (length - i >= 4U && memcmp(html + i, "<!--", 4U) == 0) {
            size_t end = GbFind(html, length, i + 4U, "-->");
            if (end == length) goto malformed;
            i = end + 3U; continue;
        }
        if (i + 1U < length && html[i + 1U] == '!') {
            size_t end = GbFind(html, length, i + 2U, ">");
            if (end == length) goto malformed;
            i = end + 1U; continue;
        }
        /* Literal less-than characters in prose are not tag starts. */
        if (i + 1U == length || (!((html[i + 1U] >= 'A' && html[i + 1U] <= 'Z') ||
            (html[i + 1U] >= 'a' && html[i + 1U] <= 'z')) && html[i + 1U] != '/')) { ++i; continue; }
        GbTag tag;
        if (GbReadTag(html, length, i, &tag) != 0) goto malformed;
        i = tag.end;
        if (GbName(tag.name, "template")) {
            if (tag.closing) { if (inert == 0U) goto malformed; --inert; }
            else { if (inert == SC_MAX_DEPTH || tag.self_closing) goto malformed; ++inert; }
            continue;
        }
        if (tag.closing) continue;
        if (GbName(tag.name, "style") || GbName(tag.name, "script") ||
            GbName(tag.name, "textarea") || GbName(tag.name, "title") ||
            GbName(tag.name, "xmp") || GbName(tag.name, "noscript")) {
            size_t end, after;
            if (GbRawEnd(html, length, i, tag.name, &end, &after) != 0) goto malformed;
            if (inert == 0U && GbName(tag.name, "style") && GbEqual(GbGet(&tag, "id"), "umicom-guide-fonts")) {
                ++fonts; ++report->checks;
                GbSpan css = {html + i, end - i};
                if (!GbFont(css, "font-family:\"Segoe UI Variable Text\",\"Segoe UI\",sans-serif") ||
                    !GbFont(css, "font-family:\"Cascadia Code\",Consolas,monospace"))
                    GbFinding(report, path, "guide.font.roles", "Use the approved installed interface and code font declarations");
            }
            i = after; continue;
        }
        if (inert != 0U) continue;
        if (GbName(tag.name, "meta") && GbEqual(GbGet(&tag, "name"), "umicom-guide-branding")) {
            ++meta;
            if (!GbEqual(GbGet(&tag, "content"), "official-assets"))
                GbFinding(report, path, "guide.marker", "The guide identity marker has an unsupported value");
        }
        if (GbName(tag.name, "link") && GbContains(GbGet(&tag, "rel"), "icon")) {
            ++ico;
            if (!GbEqual(GbGet(&tag, "type"), "image/x-icon"))
                GbFinding(report, path, "guide.icon.format", "The standalone browser identity must use ICO");
            int result = GbAsset(GbGet(&tag, "data-umicom-guide-ico"), GbGet(&tag, "href"),
                "data:image/x-icon;base64,", assets->ico, path, report);
            if (result == 2) goto failed;
        }
        if (GbName(tag.name, "img")) {
            bool wordmark = GbHas(&tag, "data-umicom-guide-logo");
            bool symbol = GbHas(&tag, "data-umicom-guide-symbol");
            if (wordmark && symbol) goto malformed;
            if (wordmark || symbol) {
                UmiSourceBrandAsset expected = assets->symbol;
                if (wordmark) {
                    ++logo;
                    GbSpan surface = GbGet(&tag, "data-umicom-guide-surface");
                    if (surface.length != 0U && !GbEqual(surface, "light") && !GbEqual(surface, "dark"))
                        GbFinding(report, path, "guide.logo.surface", "Choose an explicit light or dark wordmark surface");
                    expected = GbEqual(surface, "light") ? assets->logo_light : assets->logo_dark;
                }
                if (GbGet(&tag, "alt").length == 0U)
                    GbFinding(report, path, "guide.image.alt", "Brand images need alternative text");
                int result = GbAsset(GbGet(&tag, wordmark ? "data-umicom-guide-logo" : "data-umicom-guide-symbol"),
                    GbGet(&tag, "src"), "data:image/svg+xml;base64,", expected, path, report);
                if (result == 2) goto failed;
            } else if (GbContains(GbGet(&tag, "alt"), "Umicom"))
                GbFinding(report, path, "guide.image.unowned", "An Umicom brand image is missing its canonical asset marker");
        }
    }
    if (inert != 0U) goto malformed;
    ++report->checks;
    if (meta != 1U || logo != 1U || ico != 1U || fonts != 1U)
        GbFinding(report, path, "guide.marker.count", "Require exactly one active guide marker, wordmark, ICO and font declaration");
    return report->findings == before ? 0 : 1;
malformed:
    ScFinding(report, path, 1U, "guide.structure", "Malformed or unsupported controlled HTML structure", true);
    return 2;
failed:
    ScFinding(report, path, 1U, "guide.asset.read", "Unable to validate the bounded asset payload", true);
    return 2;
}
