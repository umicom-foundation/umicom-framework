/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/delimiter_navigation.h
 * PURPOSE: Find balanced source delimiters using bounded, toolkit-neutral text scanning.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_DELIMITER_NAVIGATION_H
#define UMICOM_EDITOR_DELIMITER_NAVIGATION_H
#include "umicom/editor/text_position.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiEditorDelimiterSyntax
    {
        UMI_EDITOR_DELIMITER_LITERAL = 0,
        UMI_EDITOR_DELIMITER_C = 1,
        UMI_EDITOR_DELIMITER_JSON = 2
    } UmiEditorDelimiterSyntax;
    typedef struct UmiEditorDelimiterPair
    {
        size_t opening_offset, closing_offset;
        /* Match records the bracket at/before the caret. Enclosing sets SIZE_MAX. */
        size_t anchor_offset;
        char opening, closing;
    } UmiEditorDelimiterPair;
    /* Inspect a stable borrowed UTF-8 view without changing it. Parentheses,
 * square brackets and braces are structural. Match prefers a structural
 * delimiter at the caret, otherwise immediately before it. Enclosing finds
 * the innermost complete pair whose opening <= caret <= closing.
 *
 * Literal mode counts every bracket. C mode skips quoted literals, line and
 * block comments, handles escaped newlines and number digit separators.
 * JSON mode skips double-quoted strings. These are lexical aids, not parsers:
 * C preprocessing, C++ raw strings, digraphs and other language grammars are
 * not interpreted. An unfinished quote/comment masks its remaining text.
 * A crossed/unmatched closer discards its open stack; later independent
 * complete pairs remain usable. Unfinished opens alone do not form a pair.
 *
 * Input is limited to eight MiB and 512 simultaneous opens. The complete
 * text must be valid UTF-8 without embedded NUL. Caret must be a valid source
 * boundary, including the end but excluding the middle of CRLF or UTF-8.
 * Failure preserves output. NOT_FOUND means no complete eligible pair.
 * No compiler, filesystem, provider or GTK dependency is required. */
    UmiStatus UmiEditorDelimiterMatch(const UmiEditorTextBufferView *view, size_t caret,
                                      UmiEditorDelimiterSyntax syntax, UmiEditorDelimiterPair *out_pair);
    UmiStatus UmiEditorDelimiterEnclosing(const UmiEditorTextBufferView *view, size_t caret,
                                          UmiEditorDelimiterSyntax syntax, UmiEditorDelimiterPair *out_pair);
#ifdef __cplusplus
}
#endif
#endif
