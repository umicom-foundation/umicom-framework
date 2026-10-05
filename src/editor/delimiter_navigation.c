/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/delimiter_navigation.c
 * PURPOSE: Scan balanced source delimiters while preserving original UTF-8 offsets and lexical boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/delimiter_navigation.h"
#include <stdlib.h>
#include <string.h>
#define DELIMITER_SOURCE_LIMIT (8U * 1024U * 1024U)
#define DELIMITER_DEPTH_LIMIT 512U
typedef struct DelimiterOpen
{
    size_t offset;
    char symbol;
} DelimiterOpen;
/* C line splicing occurs before comments and quotes are interpreted. Keep
 * the original byte offset while reading the next logical byte so a match
 * still points into the unchanged draft, including its physical newlines. */
static int DelimiterNext(const UmiEditorTextBufferView *view, UmiEditorDelimiterSyntax syntax, size_t *cursor,
                         size_t *offset)
{
    while (*cursor < view->byte_count)
    {
        size_t at = *cursor;
        if (syntax == UMI_EDITOR_DELIMITER_C && view->bytes[at] == '\\' && at + 1U < view->byte_count)
        {
            if (view->bytes[at + 1U] == '\n')
            {
                *cursor = at + 2U;
                continue;
            }
            if (view->bytes[at + 1U] == '\r')
            {
                *cursor = at + 2U + (at + 2U < view->byte_count && view->bytes[at + 2U] == '\n' ? 1U : 0U);
                continue;
            }
        }
        *offset = at;
        *cursor = at + 1U;
        return (unsigned char)view->bytes[at];
    }
    return -1;
}
static char DelimiterClose(int symbol)
{
    return symbol == '(' ? ')' : symbol == '[' ? ']' : symbol == '{' ? '}' : '\0';
}
static int DelimiterIsClose(int symbol) { return symbol == ')' || symbol == ']' || symbol == '}'; }
static int DelimiterNumberByte(int symbol)
{
    return (symbol >= '0' && symbol <= '9') || (symbol >= 'a' && symbol <= 'z') ||
           (symbol >= 'A' && symbol <= 'Z') || symbol == '_' || symbol == '.';
}
/* Identifiers can contain digits without becoming preprocessing numbers.
 * Keep that state separate so a following character literal is still quoted. */
static int DelimiterNameStart(int symbol)
{
    return (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z') || symbol == '_' ||
           symbol >= 0x80;
}
static int DelimiterNameByte(int symbol)
{
    return DelimiterNameStart(symbol) || (symbol >= '0' && symbol <= '9');
}
static UmiStatus DelimiterScan(const UmiEditorTextBufferView *view, size_t caret,
                               UmiEditorDelimiterSyntax syntax, int enclosing,
                               UmiEditorDelimiterPair *out_pair)
{
    if (view == NULL || out_pair == NULL ||
        (syntax != UMI_EDITOR_DELIMITER_LITERAL && syntax != UMI_EDITOR_DELIMITER_C &&
         syntax != UMI_EDITOR_DELIMITER_JSON))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (view->byte_count > DELIMITER_SOURCE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiEditorTextPosition position;
    UmiStatus status = UmiEditorTextViewPositionAt(view, view->byte_count, &position);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(view, caret, &position);
    if (status != UMI_STATUS_OK)
        return status;
    if (view->byte_count != 0U && memchr(view->bytes, '\0', view->byte_count) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    DelimiterOpen *stack = calloc(DELIMITER_DEPTH_LIMIT, sizeof(*stack));
    if (stack == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t cursor = 0U, depth = 0U, offset = 0U;
    int quote = 0, comment = 0, number = 0, identifier = 0;
    int seen_at = 0, found_at = 0, found_before = 0, found_enclosing = 0;
    UmiEditorDelimiterPair at_pair = {0}, before_pair = {0}, inner_pair = {0};
    status = UMI_STATUS_NOT_FOUND;
    for (;;)
    {
        int symbol = DelimiterNext(view, syntax, &cursor, &offset);
        if (symbol < 0)
            break;
        size_t peek_cursor = cursor, peek_offset = 0U;
        int next = DelimiterNext(view, syntax, &peek_cursor, &peek_offset);
        if (comment == 1)
        {
            if (symbol == '\n' || symbol == '\r')
                comment = 0;
            continue;
        }
        if (comment == 2)
        {
            if (symbol == '*' && next == '/')
            {
                cursor = peek_cursor;
                comment = 0;
            }
            continue;
        }
        if (quote != 0)
        {
            if (symbol == '\\' && next >= 0)
                cursor = peek_cursor;
            else if (symbol == quote)
                quote = 0;
            continue;
        }
        if (syntax == UMI_EDITOR_DELIMITER_C)
        {
            if (symbol == '/' && (next == '/' || next == '*'))
            {
                comment = next == '/' ? 1 : 2;
                cursor = peek_cursor;
                number = 0;
                identifier = 0;
                continue;
            }
            /* C preprocessing numbers include digit separators. Treat an
             * apostrophe within that number as a separator, not a char quote. */
            if (number && (DelimiterNumberByte(symbol) || (symbol == '\'' && DelimiterNumberByte(next))))
                continue;
            number = 0;
            if (identifier && DelimiterNameByte(symbol))
                continue;
            identifier = 0;
            if (DelimiterNameStart(symbol))
            {
                identifier = 1;
                continue;
            }
            number = symbol >= '0' && symbol <= '9';
            if (symbol == '\'' || symbol == '"')
            {
                quote = symbol;
                number = 0;
                continue;
            }
        }
        else if (syntax == UMI_EDITOR_DELIMITER_JSON && symbol == '"')
        {
            quote = symbol;
            continue;
        }
        char close = DelimiterClose(symbol);
        if (close != '\0' || DelimiterIsClose(symbol))
            if (offset == caret)
                seen_at = 1;
        if (close != '\0')
        {
            if (depth == DELIMITER_DEPTH_LIMIT)
            {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                goto done;
            }
            stack[depth++] = (DelimiterOpen){offset, (char)symbol};
            continue;
        }
        if (!DelimiterIsClose(symbol))
            continue;
        /* Mismatched nesting cannot be guessed. Retire these unfinished opens
         * and keep looking for independent pairs later in an incomplete draft. */
        if (depth == 0U || DelimiterClose(stack[depth - 1U].symbol) != (char)symbol)
        {
            depth = 0U;
            continue;
        }
        DelimiterOpen open = stack[--depth];
        UmiEditorDelimiterPair pair = {open.offset, offset, SIZE_MAX, open.symbol, (char)symbol};
        if (open.offset == caret || offset == caret)
        {
            at_pair = pair;
            at_pair.anchor_offset = caret;
            found_at = 1;
        }
        if (caret > 0U && (open.offset == caret - 1U || offset == caret - 1U))
        {
            before_pair = pair;
            before_pair.anchor_offset = caret - 1U;
            found_before = 1;
        }
        if (open.offset <= caret && caret <= offset &&
            (!found_enclosing ||
             offset - open.offset < inner_pair.closing_offset - inner_pair.opening_offset))
        {
            inner_pair = pair;
            found_enclosing = 1;
        }
    }
    if (enclosing && found_enclosing)
    {
        *out_pair = inner_pair;
        status = UMI_STATUS_OK;
    }
    else if (!enclosing && found_at)
    {
        *out_pair = at_pair;
        status = UMI_STATUS_OK;
    }
    else if (!enclosing && !seen_at && found_before)
    {
        *out_pair = before_pair;
        status = UMI_STATUS_OK;
    }
done:
    free(stack);
    return status;
}
UmiStatus UmiEditorDelimiterMatch(const UmiEditorTextBufferView *view, size_t caret,
                                  UmiEditorDelimiterSyntax syntax, UmiEditorDelimiterPair *out_pair)
{
    return DelimiterScan(view, caret, syntax, 0, out_pair);
}
UmiStatus UmiEditorDelimiterEnclosing(const UmiEditorTextBufferView *view, size_t caret,
                                      UmiEditorDelimiterSyntax syntax, UmiEditorDelimiterPair *out_pair)
{
    return DelimiterScan(view, caret, syntax, 1, out_pair);
}
