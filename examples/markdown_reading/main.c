/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/markdown_reading/main.c
 *
 * PURPOSE:
 *   Inspect owned Markdown reading text and semantic ranges through an installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/browser/reading_markdown.h"

int main(int argc, char **argv)
{
    static const char SOURCE[] = "# Umicom practice\n\nRead **owned** text.\n\n~~~c\n  int value = 1;\n~~~";
    UmiBrowserReadingDocument *document = NULL;
    UmiBrowserReadingSummary summary;
    const char *mode = argc == 2 ? argv[1] : "";
    const char *source = SOURCE;
    size_t count = sizeof(SOURCE) - 1U;
    if (argc != 2 || (strcmp(mode, "structure") != 0 &&
        strcmp(mode, "literal") != 0 && strcmp(mode, "refusal") != 0)) {
        fprintf(stderr, "usage: markdown-reading structure|literal|refusal\n"); return 2;
    }
    if (strcmp(mode, "literal") == 0) {
        source = "<script>practice</script> [link](https://example.invalid)";
        count = strlen(source);
    } else if (strcmp(mode, "refusal") == 0) { source = "a\0b"; count = 3U; }
    UmiStatus status = UmiBrowserReadingMarkdownCreate(source, count, &document);
    if (strcmp(mode, "refusal") == 0) {
        if (status == UMI_STATUS_INVALID_ARGUMENT && document == NULL) {
            puts("Invalid source refused; no partial document returned."); return 0;
        }
        UmiBrowserReadingDocumentDestroy(document); return 1;
    }
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Reading projection failed: %d\n", (int)status); return 1;
    }
    status = UmiBrowserReadingDocumentSummary(document, &summary);
    if (status != UMI_STATUS_OK || summary.source_bytes != count ||
        summary.links != 0U || summary.image_descriptions != 0U) {
        UmiBrowserReadingDocumentDestroy(document); return 1;
    }
    if ((strcmp(mode, "structure") == 0 && summary.spans != 3U) ||
        (strcmp(mode, "literal") == 0 && summary.spans != 0U)) {
        UmiBrowserReadingDocumentDestroy(document); return 1;
    }
    printf("Source bytes: %zu; reading bytes: %zu; semantic spans: %zu\n",
           summary.source_bytes, summary.text_bytes, summary.spans);
    puts(UmiBrowserReadingDocumentText(document));
    for (size_t i = 0U; i < summary.spans; ++i) {
        UmiBrowserReadingSpan span;
        status = UmiBrowserReadingDocumentSpanAt(document, i, &span);
        if (status != UMI_STATUS_OK) { UmiBrowserReadingDocumentDestroy(document); return 1; }
        printf("Range %zu: kind %u, byte offset %zu, byte length %zu\n",
               i, (unsigned int)span.kind, span.byte_start, span.byte_length);
    }
    UmiBrowserReadingDocumentDestroy(document);
    return 0;
}
