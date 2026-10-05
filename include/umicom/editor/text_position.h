/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/text_position.h
 * PURPOSE: Convert editor byte offsets and protocol text positions without lossy rounding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_TEXT_POSITION_H
#define UMICOM_EDITOR_TEXT_POSITION_H
#include "umicom/editor/text_buffer.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiEditorTextPosition
    {
        uint64_t line;
        uint64_t utf16_column;
    } UmiEditorTextPosition;
    /* Read a stable borrowed buffer view. Lines are zero-based and recognize LF,
 * CRLF and standalone CR. Columns count UTF-16 units, not UTF-8 bytes or drawn
 * glyphs. A supplementary Unicode character contributes two UTF-16 units.
 *
 * Exact positions only: no clamping to line ends, rounding inside a UTF-8
 * sequence, splitting a surrogate pair, or offset between CR and LF. The end
 * of the buffer is a valid position. UTF-8 on the scanned prefix is validated;
 * unvisited text is not inspected. Failure preserves the output. These calls
 * allocate nothing and must not race a mutation of the view's owning buffer. */
    UmiStatus UmiEditorTextViewResolvePosition(const UmiEditorTextBufferView *view,
                                               UmiEditorTextPosition position, size_t *out_byte_offset);
    UmiStatus UmiEditorTextViewPositionAt(const UmiEditorTextBufferView *view, size_t byte_offset,
                                          UmiEditorTextPosition *out_position);
#ifdef __cplusplus
}
#endif
#endif
