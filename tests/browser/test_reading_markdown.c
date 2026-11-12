/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/browser/test_reading_markdown.c
 *
 * PURPOSE:
 *   Verify supported Markdown reading structure, literal content, bounded work and refusal without a partial owner.
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
#include <string.h>
#include "umicom/base/utf8.h"
#include "umicom/browser/reading_markdown.h"
typedef struct Case {
    const char *name, *input, *text;
    size_t spans;
    UmiBrowserReadingKind first_kind;
} Case;
static const Case cases[] = {
    {"empty", "", "", 0U, 0},
    {"paragraph", "one\ntwo", "one two\n", 0U, 0},
    {"blank", "one\n\n two", "one\ntwo\n", 0U, 0},
    {"crlf", "one\r\ntwo\rthree", "one two three\n", 0U, 0},
    {"heading", "# Title", "Title\n", 1U, UMI_BROWSER_READING_HEADING},
    {"heading-indent", "   ## Title ##", "Title\n", 1U, UMI_BROWSER_READING_HEADING},
    {"heading-six", "###### Six", "Six\n", 1U, UMI_BROWSER_READING_HEADING},
    {"heading-seven", "####### Literal", "####### Literal\n", 0U, 0},
    {"heading-space", "#literal", "#literal\n", 0U, 0},
    {"heading-empty", "#", "", 0U, 0},
    {"heading-four-spaces", "    # literal", "# literal\n", 0U, 0},
    {"list", "- first\n+ second\n* third", "- first\n- second\n- third\n", 0U, 0},
    {"ordered-literal", "1. first\n2. second", "1. first 2. second\n", 0U, 0},
    {"strong", "one **two**", "one two\n", 1U, UMI_BROWSER_READING_STRONG},
    {"emphasis", "*word*", "word\n", 1U, UMI_BROWSER_READING_EMPHASIS},
    {"nested", "**a *b* c**", "a b c\n", 2U, UMI_BROWSER_READING_EMPHASIS},
    {"triple-literal", "***word***", "***word***\n", 0U, 0},
    {"unmatched", "one **two", "one **two\n", 0U, 0},
    {"space-markers", "* word *", "- word *\n", 0U, 0},
    {"underscore-literal", "_one_ __two__", "_one_ __two__\n", 0U, 0},
    {"escape", "\\*literal\\*", "*literal*\n", 0U, 0},
    {"code", "\x60x < y\x60", "x < y\n", 1U, UMI_BROWSER_READING_CODE},
    {"code-entity", "\x60&amp;\x60", "&amp;\n", 1U, UMI_BROWSER_READING_CODE},
    {"code-island", "*a \x60*\x60 b*", "a * b\n", 2U, UMI_BROWSER_READING_CODE},
    {"multi-code-literal", "\x60\x60x\x60\x60", "\x60\x60x\x60\x60\n", 0U, 0},
    {"fence", "\x60\x60\x60c\n  int x;\n\x60\x60\x60", "  int x;\n", 1U, UMI_BROWSER_READING_CODE},
    {"fence-unclosed", "~~~\n  x", "  x\n", 1U, UMI_BROWSER_READING_CODE},
    {"fence-crlf", "~~~\r\n\tx\r\n~~~", "\tx\n", 1U, UMI_BROWSER_READING_CODE},
    {"fence-marker", "~~~~\nx\n~~~\ny\n~~~~", "x\n~~~\ny\n", 1U, UMI_BROWSER_READING_CODE},
    {"fence-literal", "~~~\n**bold** <script>x</script>\n~~~", "**bold** <script>x</script>\n", 1U, UMI_BROWSER_READING_CODE},
    {"fence-close-label", "~~~\nx\n~~~label\ny\n~~~", "x\n~~~label\ny\n", 1U, UMI_BROWSER_READING_CODE},
    {"html-literal", "<script>x</script>", "<script>x</script>\n", 0U, 0},
    {"entities-literal", "&amp; &#65;", "&amp; &#65;\n", 0U, 0},
    {"link-literal", "[read](https://example.invalid)", "[read](https://example.invalid)\n", 0U, 0},
    {"image-literal", "![logo](logo.svg)", "![logo](logo.svg)\n", 0U, 0},
    {"table-literal", "| a | b |\n| - | - |", "| a | b | | - | - |\n", 0U, 0},
    {"unicode", "# \xE2\x82\xAC\n\n**\xF0\x9F\x8C\x8D**", "\xE2\x82\xAC\n\xF0\x9F\x8C\x8D\n", 2U, UMI_BROWSER_READING_HEADING}
};

int main(int argc, char **argv)
{
    UmiBrowserReadingDocument *document = NULL;
    UmiBrowserReadingSummary summary;
    const Case *selected = NULL;
    assert(argc == 2);
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(cases[i].name, argv[1]) == 0) selected = &cases[i];
    if (selected != NULL) {
        size_t count = strlen(selected->input);
        char *owned = malloc(count + 1U); assert(owned != NULL);
        memcpy(owned, selected->input, count + 1U);
        assert(UmiBrowserReadingMarkdownCreate(owned, count, &document) == UMI_STATUS_OK);
        memset(owned, 'z', count); free(owned);
        assert(UmiBrowserReadingDocumentSummary(document, &summary) == UMI_STATUS_OK);
        const char *text = UmiBrowserReadingDocumentText(document);
        assert(summary.source_bytes == count);
        assert(summary.text_bytes == strlen(selected->text));
        assert(strcmp(text, selected->text) == 0);
        assert(UmiUtf8Validate((const unsigned char *)text, summary.text_bytes, NULL));
        assert(summary.spans == selected->spans);
        assert(summary.links == 0U && summary.image_descriptions == 0U);
        for (size_t i = 0U; i < summary.spans; ++i) {
            UmiBrowserReadingSpan span;
            assert(UmiBrowserReadingDocumentSpanAt(document, i, &span) == UMI_STATUS_OK);
            assert(span.byte_start <= summary.text_bytes);
            assert(span.byte_length <= summary.text_bytes - span.byte_start);
            assert(UmiUtf8Validate((const unsigned char *)text, span.byte_start, NULL));
            assert(UmiUtf8Validate((const unsigned char *)text + span.byte_start, span.byte_length, NULL));
            if (i == 0U) assert(span.kind == selected->first_kind);
            if (strcmp(selected->name, "heading-six") == 0) assert(span.heading_level == 6U);
        }
        UmiBrowserReadingDocumentDestroy(document); return 0;
    }
    char *input = malloc(UMI_BROWSER_MARKDOWN_SOURCE_MAX + 1U); assert(input != NULL);
    size_t count = 0U;
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(argv[1], "embedded-nul") == 0) { memcpy(input, "a\0b", 3U); count = 3U; }
    else if (strcmp(argv[1], "utf8") == 0) { memcpy(input, "\xF4\x90\x80\x80", 4U); count = 4U; }
    else if (strcmp(argv[1], "control") == 0) { input[0] = 1; count = 1U; }
    else if (strcmp(argv[1], "delete") == 0) { input[0] = 0x7F; count = 1U; }
    else if (strcmp(argv[1], "over-limit") == 0) { count = UMI_BROWSER_MARKDOWN_SOURCE_MAX + 1U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(argv[1], "span-capacity") == 0) {
        for (size_t i = 0U; i <= UMI_BROWSER_READING_SPANS_MAX; ++i) { memcpy(input + count, "*x*\n\n", 5U); count += 5U; }
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(argv[1], "work-capacity") == 0) {
        for (size_t i = 0U; i < 5000U; ++i) { memcpy(input + count, "*a ", 3U); count += 3U; }
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(argv[1], "exact-span") == 0) {
        free(input); input = malloc(2U); assert(input != NULL);
        input[0] = 'o'; input[1] = 'k'; count = 2U; expected = UMI_STATUS_OK;
    } else if (strcmp(argv[1], "source-limit") == 0) {
        count = UMI_BROWSER_MARKDOWN_SOURCE_MAX; memset(input, 'x', count); expected = UMI_STATUS_OK;
    } else if (strcmp(argv[1], "null-source") == 0) {
        assert(UmiBrowserReadingMarkdownCreate(NULL, 0U, &document) == UMI_STATUS_INVALID_ARGUMENT);
        free(input); return 0;
    } else if (strcmp(argv[1], "null-output") == 0) {
        assert(UmiBrowserReadingMarkdownCreate("", 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        free(input); return 0;
    } else assert(0);
    /* A sentinel detects publication of an incomplete owner on a failed parse. */
    document = (UmiBrowserReadingDocument *)(void *)input;
    assert(UmiBrowserReadingMarkdownCreate(input, count, &document) == expected);
    if (expected != UMI_STATUS_OK) assert(document == NULL);
    else {
        assert(UmiBrowserReadingDocumentSummary(document, &summary) == UMI_STATUS_OK);
        assert(summary.source_bytes == count && summary.text_bytes == count + 1U);
        UmiBrowserReadingDocumentDestroy(document);
    }
    free(input); return 0;
}
