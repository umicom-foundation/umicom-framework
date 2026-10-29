/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/reading_search/main.c
 *
 * PURPOSE:
 *   Find successive literal matches in an owned guide using installed Browser and Editor services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/browser/reading_document.h"
#include "umicom/editor/search_engine.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    static const char html[] = "<h1>Read a guide</h1><p>Framework services share behaviour.</p>"
        "<p>A framework guide can explain that behaviour.</p><script>Framework hidden</script>";
    static const char query[] = "framework";
    UmiBrowserReadingDocument *document = NULL;
    UmiStatus status = UmiBrowserReadingDocumentCreate(html,sizeof(html)-1U,&document);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr,"Cannot prepare guide: %d\n",(int)status); return 1;
    }
    UmiBrowserReadingSummary summary;
    status = UmiBrowserReadingDocumentSummary(document,&summary);
    const char *text = UmiBrowserReadingDocumentText(document);
    if (status != UMI_STATUS_OK || text == NULL || strstr(text,"hidden") != NULL) {
        UmiBrowserReadingDocumentDestroy(document); return 1;
    }
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE,0,0,0U};
    size_t start = 0U, first = 0U, previous = 0U;
    puts(text);
    for (unsigned int step = 0U; step < 3U; ++step) {
        UmiEditorSearchMatch match;
        int wrapped = 0;
        status = UmiEditorSearchNavigate(text,summary.text_bytes,query,sizeof(query)-1U,
            &options,start,0,1,&match,&wrapped);
        if (status != UMI_STATUS_OK) break;
        if ((step < 2U && wrapped) || (step == 1U && match.offset <= previous) ||
            (step == 2U && (!wrapped || match.offset != first))) {
            status = UMI_STATUS_INVALID_STATE; break;
        }
        if (step == 0U) first = match.offset;
        printf("Match %u: byte %zu, length %zu, wrapped %d: %.*s\n",
            step+1U,match.offset,match.byte_count,wrapped,(int)match.byte_count,text+match.offset);
        previous = match.offset;
        /* Moving past the complete match prevents Next from selecting it again.
         * The matcher already proved that this range lies inside the text. */
        start = match.offset + match.byte_count;
    }
    /* Text is borrowed from the document, so finish every search before freeing
     * its owner. A GUI additionally converts these byte offsets to characters. */
    UmiBrowserReadingDocumentDestroy(document);
    if (status != UMI_STATUS_OK) fprintf(stderr,"Search refused: %d\n",(int)status);
    return status == UMI_STATUS_OK ? 0 : 1;
}
