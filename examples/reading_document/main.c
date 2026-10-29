/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reading_document/main.c
 *
 * PURPOSE:
 *   Show copied reading metadata and borrowed text from an installed Framework SDK.
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

/* This example processes an in-memory document. It does not read a file,
 * connect to a network, execute code or follow a link. An application must
 * separately admit its content and decide which explicit links are allowed. */
int main(void)
{
    static const char html[] =
        "<h1 id='start'>Reading a C example</h1>"
        "<p>Open the <a href='#source'>source section</a>.</p>"
        "<h2 id='source'>Source</h2><pre><code>int answer(void)\n"
        "{\n    return 42;\n}</code></pre>";
    UmiBrowserReadingDocument *document = NULL;
    UmiStatus status = UmiBrowserReadingDocumentCreate(html, sizeof(html)-1U, &document);
    if (status != UMI_STATUS_OK) { fprintf(stderr, "Reading model refused: %d\n", (int)status); return 1; }
    UmiBrowserReadingSummary summary;
    status = UmiBrowserReadingDocumentSummary(document, &summary);
    const char *text = UmiBrowserReadingDocumentText(document);
    if (status != UMI_STATUS_OK || text == NULL ||
        strstr(text, "{\n    return 42;\n}") == NULL || summary.links != 1U) {
        UmiBrowserReadingDocumentDestroy(document); return 1;
    }
    printf("%s\n", text);
    printf("Source bytes: %zu; reading bytes: %zu; ranges: %zu; links: %zu\n",
        summary.source_bytes, summary.text_bytes, summary.spans, summary.links);
    UmiBrowserReadingLink link;
    status = UmiBrowserReadingDocumentLinkAt(document, 0U, &link);
    size_t offset = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiBrowserReadingDocumentAnchor(document, "source", &offset);
    if (status == UMI_STATUS_OK) {
        printf("Explicit link target: %s; source section byte offset: %zu\n", link.target, offset);
        if (strcmp(link.target, "#source") != 0 || strncmp(text + offset, "Source", 6U) != 0)
            status = UMI_STATUS_INVALID_STATE;
    }
    /* Borrowed text must not be read after destroying its owner. Metadata
     * copied into link and summary is ordinary independent value data. */
    UmiBrowserReadingDocumentDestroy(document);
    return status == UMI_STATUS_OK ? 0 : 1;
}
