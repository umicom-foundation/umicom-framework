/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/reading_markdown.c
 *
 * PURPOSE:
 *   Admit Unicode Markdown and reuse Framework reading ownership through escaped, bounded structural projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "reading_markdown_internal.h"
#include "reading_document_internal.h"
#include "umicom/base/utf8.h"
#include <stdlib.h>
#include <string.h>

static int Space(char ch) { return ch == ' ' || ch == '\t'; }
static int Blank(const char *text, size_t bytes)
{
    for (size_t i = 0U; i < bytes; ++i) if (!Space(text[i])) return 0;
    return 1;
}
static void CloseParagraph(MarkdownWriter *writer, int *paragraph)
{
    if (*paragraph) MarkdownLiteral(writer, "</p>");
    *paragraph = 0;
}
static size_t Fence(const char *text, size_t bytes, char *marker)
{
    size_t count = 0U;
    if (bytes == 0U || (text[0] != '\x60' && text[0] != '~')) return 0U;
    while (count < bytes && text[count] == text[0]) ++count;
    if (count < 3U) return 0U;
    *marker = text[0]; return count;
}
static void Build(MarkdownWriter *writer, const char *source, size_t bytes)
{
    size_t p = 0U, fence_count = 0U;
    char fence_marker = '\0';
    int paragraph = 0;
    while (p < bytes && MarkdownSpend(writer)) {
        size_t begin = p;
        while (p < bytes && source[p] != '\r' && source[p] != '\n') ++p;
        size_t length = p - begin;
        int newline = p < bytes;
        if (newline && source[p++] == '\r' && p < bytes && source[p] == '\n') ++p;
        const char *line = source + begin;
        size_t indent = 0U;
        while (indent < length && indent < 4U && line[indent] == ' ') ++indent;
        size_t prefix = indent <= 3U ? indent : 0U;
        const char *trimmed = line + prefix;
        size_t available = length - prefix;
        char marker = '\0';
        size_t count = indent <= 3U ? Fence(trimmed, available, &marker) : 0U;
        if (fence_count != 0U) {
            if (count >= fence_count && marker == fence_marker &&
                Blank(trimmed + count, available - count)) {
                MarkdownLiteral(writer, "</pre>"); fence_count = 0U;
            } else {
                MarkdownEscape(writer, line, length);
                if (newline) MarkdownLiteral(writer, "\n");
            }
            continue;
        }
        if (count != 0U) {
            /* The optional fence label is descriptive only; no language server,
             * toolchain or evaluator is invoked for a code block. */
            CloseParagraph(writer, &paragraph);
            fence_count = count; fence_marker = marker;
            MarkdownLiteral(writer, "<pre>");
            continue;
        }
        if (Blank(line, length)) { CloseParagraph(writer, &paragraph); continue; }
        size_t heading = 0U;
        if (indent <= 3U) {
            while (heading < available && trimmed[heading] == '#') ++heading;
        }
        if (heading >= 1U && heading <= 6U &&
            (heading == available || Space(trimmed[heading]))) {
            CloseParagraph(writer, &paragraph);
            size_t start = heading, end = available;
            while (start < end && Space(trimmed[start])) ++start;
            while (end > start && Space(trimmed[end - 1U])) --end;
            size_t hashes = end;
            while (hashes > start && trimmed[hashes - 1U] == '#') --hashes;
            if (hashes < end && hashes > start && Space(trimmed[hashes - 1U])) {
                end = hashes;
                while (end > start && Space(trimmed[end - 1U])) --end;
            }
            char open[] = "<h1>", close[] = "</h1>";
            open[2] = (char)('0' + heading); close[3] = open[2];
            MarkdownLiteral(writer, open);
            MarkdownInline(writer, trimmed + start, end - start, 0U);
            MarkdownLiteral(writer, close);
            continue;
        }
        if (indent == 0U && available >= 2U &&
            (trimmed[0] == '-' || trimmed[0] == '+' || trimmed[0] == '*') &&
            Space(trimmed[1])) {
            CloseParagraph(writer, &paragraph);
            size_t start = 2U;
            while (start < available && Space(trimmed[start])) ++start;
            MarkdownLiteral(writer, "<li>");
            MarkdownInline(writer, trimmed + start, available - start, 0U);
            MarkdownLiteral(writer, "</li>");
            continue;
        }
        if (paragraph) MarkdownLiteral(writer, " ");
        else { MarkdownLiteral(writer, "<p>"); paragraph = 1; }
        MarkdownInline(writer, line, length, 0U);
    }
    CloseParagraph(writer, &paragraph);
    if (fence_count != 0U) MarkdownLiteral(writer, "</pre>");
}
UmiStatus UmiBrowserReadingMarkdownCreate(const void *source, size_t bytes,
    UmiBrowserReadingDocument **out_document)
{
    if (out_document != NULL) *out_document = NULL;
    if (source == NULL || out_document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (bytes > UMI_BROWSER_MARKDOWN_SOURCE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    const unsigned char *input = source;
    if (!UmiUtf8Validate(input, bytes, NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < bytes; ++i)
        if ((input[i] < 0x20U && input[i] != '\t' && input[i] != '\r' && input[i] != '\n') ||
            input[i] == 0x7FU) return UMI_STATUS_INVALID_ARGUMENT;
    MarkdownWriter *writer = malloc(sizeof(*writer));
    if (writer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    writer->used = 0U; writer->work_left = bytes * 32U + 1024U;
    writer->status = UMI_STATUS_OK; writer->html[0] = '\0';
    Build(writer, source, bytes);
    UmiStatus status = writer->status;
    if (status == UMI_STATUS_OK)
        status = UmiBrowserReadingDocumentCreate(writer->html, writer->used, out_document);
    if (status == UMI_STATUS_OK) {
        /* The shared owner records the caller's original format identity;
         * generated structural markup never masquerades as source byte count. */
        (*out_document)->summary.source_bytes = bytes;
    }
    free(writer);
    return status;
}
