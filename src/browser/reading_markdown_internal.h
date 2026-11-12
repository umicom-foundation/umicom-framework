/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/reading_markdown_internal.h
 *
 * PURPOSE:
 *   Share the bounded Markdown serializer and scanning budget between block and inline readers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_READING_MARKDOWN_INTERNAL_H
#define UMICOM_READING_MARKDOWN_INTERNAL_H
#include "umicom/browser/reading_markdown.h"
typedef struct MarkdownWriter {
    char html[UMI_BROWSER_READING_SOURCE_MAX + 1U];
    size_t used, work_left;
    UmiStatus status;
} MarkdownWriter;
/* Sticky status makes every later append a no-op after the first refusal. */
void MarkdownAppend(MarkdownWriter *writer, const char *text, size_t bytes);
void MarkdownLiteral(MarkdownWriter *writer, const char *text);
void MarkdownEscape(MarkdownWriter *writer, const char *text, size_t bytes);
int MarkdownSpend(MarkdownWriter *writer);
void MarkdownInline(MarkdownWriter *writer, const char *text, size_t bytes, unsigned int depth);
#endif
