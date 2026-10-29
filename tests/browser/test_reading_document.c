/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/browser/test_reading_document.c
 *
 * PURPOSE:
 *   Verify document structure, bounded parsing, refused markup and independently owned reading text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/browser/reading_document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); result = 1; goto cleanup; } } while (0)
typedef struct ReadingCase {
    const char *name, *html, *text;
    UmiStatus status;
    size_t spans, links, anchors, images, missing;
} ReadingCase;
static const ReadingCase cases[] = {
    {"empty", "", "", UMI_STATUS_OK,0,0,0,0,0},
    {"paragraphs", "<p>Read   this.</p><p>Then\tcontinue.</p>", "Read this.\nThen continue.\n", UMI_STATUS_OK,0,0,0,0,0},
    {"headings", "<h1 id='first'>First</h1><h2>Second</h2>", "First\nSecond\n", UMI_STATUS_OK,2,0,1,0,0},
    {"code", "<pre><code>int main(void)\r\n{\r\n    return 0;\r\n}</code></pre>", "int main(void)\n{\n    return 0;\n}\n", UMI_STATUS_OK,2,0,0,0,0},
    {"inline", "<p>Use <strong>C</strong> and <em>care</em>.</p>", "Use C and care.\n", UMI_STATUS_OK,2,0,0,0,0},
    {"nested", "<h2><strong>Owned <em>text</em></strong></h2>", "Owned text\n", UMI_STATUS_OK,3,0,0,0,0},
    {"entities", "<p>&lt;&amp;&gt;&quot;&apos;&nbsp;&#65;&#x3A9;&#x1F30D;</p>", "<&>\"' A\xce\xa9\xf0\x9f\x8c\x8d\n", UMI_STATUS_OK,0,0,0,0,0},
    {"unknown-entity", "&unrecognised; remains", "&unrecognised; remains", UMI_STATUS_OK,0,0,0,0,0},
    {"comments", "<!-- <script>not markup</script> -->Visible", "Visible", UMI_STATUS_OK,0,0,0,0,0},
    {"hidden", "<div hidden>secret<script>if (x < 2) hidden()</script><div>nested</div></div>Visible", "Visible", UMI_STATUS_OK,0,0,0,0,0},
    {"script", "<script>const x = '</style>'; if (x < 2) work();</script>Visible", "Visible", UMI_STATUS_OK,0,0,0,0,0},
    {"head", "<!DOCTYPE html><html><head><title>Title</title><style>hidden</style><script>const x = '</head>';</script></head><body>Visible</body></html>", "Visible", UMI_STATUS_OK,0,0,0,0,0},
    {"template", "<template>outer<template>nested</template>end</template>Visible", "Visible", UMI_STATUS_OK,0,0,0,0,0},
    {"svg", "<svg><text>Not a text substitute</text></svg>Visible", "Visible", UMI_STATUS_OK,0,0,0,0,0},
    {"external", "<iframe src='https://invalid.example/'>Fallback</iframe><object data='file:///private'>Hidden</object>Visible", "Visible", UMI_STATUS_OK,0,0,0,0,0},
    {"table", "<table><tr><th>Name</th><th>Meaning</th></tr><tr><td>Owner</td><td>Lifetime</td></tr></table>", "Name | Meaning\nOwner | Lifetime\n", UMI_STATUS_OK,0,0,0,0,0},
    {"list", "<ol><li>Open</li><li>Read</li></ol>", "- Open\n- Read\n", UMI_STATUS_OK,0,0,0,0,0},
    {"image", "<img src='https://invalid.example/image' alt='Owned &amp; borrowed'/>", "[Image: Owned & borrowed]", UMI_STATUS_OK,1,0,0,1,0},
    {"image-missing", "<img src='file:///private'>", "[Image: description missing]", UMI_STATUS_OK,1,0,0,1,1},
    {"image-decorative", "<img alt=''>", "[Image: decorative]", UMI_STATUS_OK,1,0,0,1,0},
    {"links", "<a href='guide.html#owned'>Owned</a> <a href='https://invalid.example/'>Reference</a>", "Owned Reference", UMI_STATUS_OK,0,2,0,0,0},
    {"quoted-angle", "<p title='a > b'>Shown</p>", "Shown\n", UMI_STATUS_OK,0,0,0,0,0},
    {"unquoted", "<a href=guide.html id=guide>Guide</a>", "Guide", UMI_STATUS_OK,0,1,1,0,0},
    {"anchor-entity", "<h2 id='own&#101;d'>Owned</h2>", "Owned\n", UMI_STATUS_OK,1,0,1,0,0},
    {"unsupported", "<custom-panel>Text</custom-panel>", "Text", UMI_STATUS_OK,0,0,0,0,0},
    {"mismatch", "<strong><em>text</strong></em>", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"unclosed-style", "<pre>text", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"unclosed-script", "<script>text", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"unclosed-comment", "<!-- hidden", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"unclosed-quote", "<a href='guide>text</a>", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"duplicate-anchor", "<p id='one'>A</p><p id='one'>B</p>", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"duplicate-attribute", "<a href='first' HREF='second'>Text</a>", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"anchor-space", "<p id='not one'>Text</p>", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"numeric-nul", "&#0;", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"numeric-surrogate", "&#xd800;", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"numeric-overflow", "&#x110000;", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"numeric-control", "&#x1b;", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0},
    {"external-doctype", "<!DOCTYPE html SYSTEM 'file:///private'>Text", NULL, UMI_STATUS_INVALID_ARGUMENT,0,0,0,0,0}
};
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int result = 0, matched = 0;
    UmiBrowserReadingDocument *document = NULL;
    char *storage = NULL;
    UmiBrowserReadingSummary summary;
    for (size_t c = 0U; c < sizeof(cases)/sizeof(cases[0]); ++c) {
        const ReadingCase *item = &cases[c];
        if (strcmp(argv[1], item->name) != 0) continue;
        matched = 1;
        size_t length = strlen(item->html);
        storage = malloc(length + 1U); CHECK(storage != NULL);
        memcpy(storage, item->html, length + 1U);
        CHECK(UmiBrowserReadingDocumentCreate(storage, length, &document) == item->status);
        memset(storage, 'x', length); free(storage); storage = NULL;
        if (item->status != UMI_STATUS_OK) { CHECK(document == NULL); goto cleanup; }
        CHECK(UmiBrowserReadingDocumentSummary(document, &summary) == UMI_STATUS_OK);
        CHECK(strcmp(UmiBrowserReadingDocumentText(document), item->text) == 0);
        CHECK(summary.source_bytes == length && summary.text_bytes == strlen(item->text));
        CHECK(summary.spans == item->spans && summary.links == item->links && summary.anchors == item->anchors);
        CHECK(summary.image_descriptions == item->images && summary.missing_image_descriptions == item->missing);
        for (size_t i = 0U; i < summary.spans; ++i) {
            UmiBrowserReadingSpan span;
            CHECK(UmiBrowserReadingDocumentSpanAt(document, i, &span) == UMI_STATUS_OK);
            CHECK(span.byte_start <= summary.text_bytes && span.byte_length <= summary.text_bytes - span.byte_start);
        }
        if (strcmp(item->name, "headings") == 0) {
            UmiBrowserReadingSpan span;
            CHECK(UmiBrowserReadingDocumentSpanAt(document, 0U, &span) == UMI_STATUS_OK);
            CHECK(span.kind == UMI_BROWSER_READING_HEADING && span.heading_level == 1U &&
                span.byte_start == 0U && span.byte_length == 5U);
        }
        if (strcmp(item->name, "anchor-entity") == 0) {
            size_t offset = 999U;
            CHECK(UmiBrowserReadingDocumentAnchor(document, "owned", &offset) == UMI_STATUS_OK && offset == 0U);
            offset = 999U;
            CHECK(UmiBrowserReadingDocumentAnchor(document, "Owned", &offset) == UMI_STATUS_NOT_FOUND && offset == 999U);
        }
        if (strcmp(item->name, "links") == 0) {
            UmiBrowserReadingLink link;
            CHECK(UmiBrowserReadingDocumentLinkAt(document, 1U, &link) == UMI_STATUS_OK);
            CHECK(link.byte_start == 6U && link.byte_length == 9U);
            CHECK(strcmp(link.target, "https://invalid.example/") == 0);
        }
        goto cleanup;
    }
    if (strcmp(argv[1], "limits") == 0) {
        matched = 1;
        storage = malloc(UMI_BROWSER_READING_SOURCE_MAX + 1U); CHECK(storage != NULL);
        memset(storage, 'x', UMI_BROWSER_READING_SOURCE_MAX + 1U);
        CHECK(UmiBrowserReadingDocumentCreate(storage, UMI_BROWSER_READING_SOURCE_MAX + 1U, &document) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(document == NULL);
        CHECK(UmiBrowserReadingDocumentCreate(storage, UMI_BROWSER_READING_TEXT_MAX + 1U, &document) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(document == NULL);
        CHECK(UmiBrowserReadingDocumentCreate(storage, UMI_BROWSER_READING_TEXT_MAX, &document) == UMI_STATUS_OK);
        CHECK(strlen(UmiBrowserReadingDocumentText(document)) == UMI_BROWSER_READING_TEXT_MAX);
    } else if (strcmp(argv[1], "arguments") == 0) {
        matched = 1;
        CHECK(UmiBrowserReadingDocumentCreate(NULL, 0U, &document) == UMI_STATUS_INVALID_ARGUMENT && document == NULL);
        CHECK(UmiBrowserReadingDocumentCreate("", 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrowserReadingDocumentCreate("a\0b", 3U, &document) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrowserReadingDocumentCreate("a\x1b", 2U, &document) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrowserReadingDocumentCreate("hello", 5U, &document) == UMI_STATUS_OK);
        UmiBrowserReadingSpan span = {UMI_BROWSER_READING_CODE, 99U, 88U, 7U};
        CHECK(UmiBrowserReadingDocumentSpanAt(document, 0U, &span) == UMI_STATUS_NOT_FOUND);
        CHECK(span.kind == UMI_BROWSER_READING_CODE && span.byte_start == 99U && span.byte_length == 88U && span.heading_level == 7U);
        CHECK(UmiBrowserReadingDocumentSpanAt(NULL, 0U, &span) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrowserReadingDocumentText(NULL) == NULL);
        UmiBrowserReadingDocumentDestroy(NULL);
    } else if (strcmp(argv[1], "span-capacity") == 0 || strcmp(argv[1], "link-capacity") == 0 ||
        strcmp(argv[1], "depth") == 0 || strcmp(argv[1], "anchor-capacity") == 0) {
        matched = 1;
        storage = calloc(1U, 32768U); CHECK(storage != NULL);
        if (strcmp(argv[1], "depth") == 0) {
            for (size_t i = 0U; i < 65U; ++i) strcat(storage, "<strong>");
        } else if (strcmp(argv[1], "anchor-capacity") == 0) {
            size_t length = 0U;
            for (size_t i = 0U; i < UMI_BROWSER_READING_ANCHORS_MAX + 1U; ++i) {
                int n = snprintf(storage + length, 32768U - length, "<p id='anchor%zu'>x</p>", i);
                CHECK(n > 0 && (size_t)n < 32768U - length); length += (size_t)n;
            }
        } else {
            const char *part = strcmp(argv[1], "span-capacity") == 0 ? "<strong>x</strong>" : "<a href='x'>x</a>";
            size_t count = strcmp(argv[1], "span-capacity") == 0 ? UMI_BROWSER_READING_SPANS_MAX : UMI_BROWSER_READING_LINKS_MAX;
            for (size_t i = 0U; i <= count; ++i) strcat(storage, part);
        }
        CHECK(UmiBrowserReadingDocumentCreate(storage, strlen(storage), &document) == UMI_STATUS_CAPACITY_EXCEEDED && document == NULL);
    }
    CHECK(matched);
cleanup:
    free(storage); UmiBrowserReadingDocumentDestroy(document);
    return result;
}
