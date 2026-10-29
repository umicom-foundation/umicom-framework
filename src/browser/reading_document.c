/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/reading_document.c
 *
 * PURPOSE:
 *   Own immutable reading text and copied semantic ranges independently of source bytes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "reading_document_internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiBrowserReadingDocumentCreate(const void *html, size_t bytes,
    UmiBrowserReadingDocument **out_document)
{
    if (out_document != NULL) *out_document = NULL;
    if (html == NULL || out_document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (bytes > UMI_BROWSER_READING_SOURCE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    const unsigned char *source = html;
    for (size_t i = 0U; i < bytes; ++i)
        if ((source[i] < 0x20U && source[i] != '\t' && source[i] != '\r' && source[i] != '\n') ||
            source[i] == 0x7fU) return UMI_STATUS_INVALID_ARGUMENT;
    UmiBrowserReadingDocument *document = calloc(1U, sizeof(*document));
    if (document == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    document->summary.source_bytes = bytes;
    UmiStatus status = ReadingBuild(html, bytes, document);
    if (status != UMI_STATUS_OK) { free(document); return status; }
    *out_document = document;
    return UMI_STATUS_OK;
}
void UmiBrowserReadingDocumentDestroy(UmiBrowserReadingDocument *document) { free(document); }
const char *UmiBrowserReadingDocumentText(const UmiBrowserReadingDocument *document)
{ return document == NULL ? NULL : document->text; }
UmiStatus UmiBrowserReadingDocumentSummary(const UmiBrowserReadingDocument *document,
    UmiBrowserReadingSummary *out_summary)
{
    if (document == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = document->summary; return UMI_STATUS_OK;
}
UmiStatus UmiBrowserReadingDocumentSpanAt(const UmiBrowserReadingDocument *document,
    size_t index, UmiBrowserReadingSpan *out_span)
{
    if (document == NULL || out_span == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= document->summary.spans) return UMI_STATUS_NOT_FOUND;
    *out_span = document->spans[index]; return UMI_STATUS_OK;
}
UmiStatus UmiBrowserReadingDocumentLinkAt(const UmiBrowserReadingDocument *document,
    size_t index, UmiBrowserReadingLink *out_link)
{
    if (document == NULL || out_link == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= document->summary.links) return UMI_STATUS_NOT_FOUND;
    *out_link = document->links[index]; return UMI_STATUS_OK;
}
UmiStatus UmiBrowserReadingDocumentAnchor(const UmiBrowserReadingDocument *document,
    const char *identifier, size_t *out_byte_offset)
{
    if (document == NULL || identifier == NULL || out_byte_offset == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_BROWSER_READING_ANCHOR_CAPACITY && identifier[length] != '\0') ++length;
    if (length == 0U || length == UMI_BROWSER_READING_ANCHOR_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < document->summary.anchors; ++i)
        if (strcmp(identifier, document->anchors[i].id) == 0) {
            *out_byte_offset = document->anchors[i].offset; return UMI_STATUS_OK;
        }
    return UMI_STATUS_NOT_FOUND;
}
