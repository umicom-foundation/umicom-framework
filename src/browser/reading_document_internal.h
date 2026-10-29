/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/reading_document_internal.h
 *
 * PURPOSE:
 *   Define an owned bounded reading projection without executing document content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROWSER_READING_DOCUMENT_INTERNAL_H
#define UMICOM_BROWSER_READING_DOCUMENT_INTERNAL_H
#include "umicom/browser/reading_document.h"
typedef struct ReadingAnchor {
    size_t offset;
    char id[UMI_BROWSER_READING_ANCHOR_CAPACITY];
} ReadingAnchor;
struct UmiBrowserReadingDocument {
    UmiBrowserReadingSummary summary;
    char text[UMI_BROWSER_READING_TEXT_MAX + 1U];
    UmiBrowserReadingSpan spans[UMI_BROWSER_READING_SPANS_MAX];
    UmiBrowserReadingLink links[UMI_BROWSER_READING_LINKS_MAX];
    ReadingAnchor anchors[UMI_BROWSER_READING_ANCHORS_MAX];
    UmiBrowserReadingImage images[UMI_BROWSER_READING_SPANS_MAX];
};
typedef struct ReadingTag {
    char name[32];
    const char *attributes;
    size_t attributes_size;
    int closing, self_closing;
} ReadingTag;
int ReadingSpace(unsigned char ch);
int ReadingEqual(const char *a, size_t count, const char *b);
UmiStatus ReadingParseTag(const char *bytes, size_t size, size_t *cursor, ReadingTag *tag);
UmiStatus ReadingAttribute(const ReadingTag *tag, const char *name,
    char *output, size_t capacity, int *found);
/* Decode one entity or copy one byte. The caller owns sufficient output room.
 * Numeric entities expand to at most four UTF-8 bytes. */
UmiStatus ReadingDecode(const char *bytes, size_t size, size_t *consumed,
    char output[4], size_t *written);
UmiStatus ReadingBuild(const char *html, size_t size, UmiBrowserReadingDocument *document);
#endif
