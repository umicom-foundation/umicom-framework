/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/reading_markdown_inline.c
 *
 * PURPOSE:
 *   Escape literal document bytes and project bounded inline code and star emphasis into the shared reader.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "reading_markdown_internal.h"
#include <string.h>

void MarkdownAppend(MarkdownWriter *writer, const char *text, size_t bytes)
{
    if (writer->status != UMI_STATUS_OK) return;
    if (bytes > UMI_BROWSER_READING_SOURCE_MAX - writer->used) {
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return;
    }
    memcpy(writer->html + writer->used, text, bytes);
    writer->used += bytes;
    writer->html[writer->used] = '\0';
}
void MarkdownLiteral(MarkdownWriter *writer, const char *text)
{ MarkdownAppend(writer, text, strlen(text)); }

void MarkdownEscape(MarkdownWriter *writer, const char *text, size_t bytes)
{
    /* Generated tags belong to Framework. Every source byte is escaped before
     * entering the existing HTML reading builder, including source entities. */
    for (size_t i = 0U; i < bytes && writer->status == UMI_STATUS_OK; ++i) {
        if (text[i] == '&') MarkdownLiteral(writer, "&amp;");
        else if (text[i] == '<') MarkdownLiteral(writer, "&lt;");
        else if (text[i] == '>') MarkdownLiteral(writer, "&gt;");
        else if (text[i] == '"') MarkdownLiteral(writer, "&quot;");
        else if (text[i] == '\'') MarkdownLiteral(writer, "&apos;");
        else MarkdownAppend(writer, text + i, 1U);
    }
}
int MarkdownSpend(MarkdownWriter *writer)
{
    if (writer->status != UMI_STATUS_OK) return 0;
    if (writer->work_left == 0U) {
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return 0;
    }
    --writer->work_left;
    return 1;
}
static int InlineSpace(char ch) { return ch == ' ' || ch == '\t'; }
static int Punctuation(unsigned char ch)
{
    return (ch >= 0x21U && ch <= 0x2FU) || (ch >= 0x3AU && ch <= 0x40U) ||
           (ch >= 0x5BU && ch <= 0x60U) || (ch >= 0x7BU && ch <= 0x7EU);
}
static size_t Run(MarkdownWriter *writer, const char *text, size_t bytes, size_t position)
{
    size_t end = position;
    while (end < bytes && text[end] == text[position] && MarkdownSpend(writer)) ++end;
    return end - position;
}
/* Search only within one physical line. A shared work budget prevents repeated
 * unsuccessful delimiter searches from producing unbounded quadratic work. */
static size_t Closing(MarkdownWriter *writer, const char *text, size_t bytes,
    size_t start, char delimiter, size_t run)
{
    size_t p = start;
    while (p < bytes && MarkdownSpend(writer)) {
        if (delimiter != '\x60' && text[p] == '\\' && p + 1U < bytes) {
            p += 2U; continue;
        }
        /* Code spans are literal islands even inside an emphasized phrase. */
        if (delimiter != '\x60' && text[p] == '\x60') {
            size_t count = Run(writer, text, bytes, p);
            if (count == 1U) {
                size_t end = Closing(writer, text, bytes, p + 1U, '\x60', 1U);
                if (end < bytes) { p = end + 1U; continue; }
            }
            p += count; continue;
        }
        if (text[p] == delimiter) {
            size_t count = Run(writer, text, bytes, p);
            if (count == run && p > start &&
                (delimiter == '\x60' || !InlineSpace(text[p - 1U])))
                return p;
            p += count;
        } else ++p;
    }
    return bytes;
}
void MarkdownInline(MarkdownWriter *writer, const char *text, size_t bytes, unsigned int depth)
{
    size_t p = 0U;
    if (depth > 16U) { writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return; }
    while (p < bytes && MarkdownSpend(writer)) {
        if (text[p] == '\\' && p + 1U < bytes && Punctuation((unsigned char)text[p + 1U])) {
            MarkdownEscape(writer, text + p + 1U, 1U); p += 2U; continue;
        }
        if (text[p] == '*' || text[p] == '\x60') {
            size_t run = Run(writer, text, bytes, p);
            int supported = text[p] == '*' ? run <= 2U : run == 1U;
            if (supported && p + run < bytes &&
                (text[p] == '\x60' || !InlineSpace(text[p + run]))) {
                size_t close = Closing(writer, text, bytes, p + run, text[p], run);
                if (writer->status != UMI_STATUS_OK) return;
                if (close < bytes) {
                    const char *open = text[p] == '\x60' ? "<code>" : run == 2U ? "<strong>" : "<em>";
                    const char *end = text[p] == '\x60' ? "</code>" : run == 2U ? "</strong>" : "</em>";
                    MarkdownLiteral(writer, open);
                    if (text[p] == '\x60') MarkdownEscape(writer, text + p + run, close - p - run);
                    else MarkdownInline(writer, text + p + run, close - p - run, depth + 1U);
                    MarkdownLiteral(writer, end);
                    p = close + run; continue;
                }
            }
            MarkdownEscape(writer, text + p, run); p += run; continue;
        }
        MarkdownEscape(writer, text + p, 1U);
        ++p;
    }
}
