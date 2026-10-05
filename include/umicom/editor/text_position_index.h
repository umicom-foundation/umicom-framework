/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/text_position_index.h
 * PURPOSE: Resolve repeated protocol positions against one owned immutable text snapshot.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_TEXT_POSITION_INDEX_H
#define UMICOM_EDITOR_TEXT_POSITION_INDEX_H
#include "umicom/editor/text_position.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiEditorTextPositionIndex UmiEditorTextPositionIndex;
    /* Copy and validate a stable source view, then build sparse coordinate
 * checkpoints. The caller can release or change its input after return.
 * Source is limited to 16 MiB. Empty views are valid. Embedded NUL remains an
 * ordinary scalar, matching the existing text-view coordinate service.
 * Cancellation is checked between bounded scan intervals, not during allocation
 * or the initial copy. Failure clears out_index and releases all owned storage.
 * Use one index per immutable draft; never reuse it for a different revision. */
    UmiStatus UmiEditorTextPositionIndexCreate(const UmiEditorTextBufferView *view,
                                               const UmiCancellationToken *cancel,
                                               UmiEditorTextPositionIndex **out_index);
    void UmiEditorTextPositionIndexDestroy(UmiEditorTextPositionIndex *index);
    /* Exact UTF-8 byte / UTF-16 coordinate conversion, with the same LF, CRLF and
 * CR rules as text_position.h. Binary search selects a checkpoint and a short
 * local scan resolves the boundary. Never clamp or split a Unicode scalar,
 * surrogate pair or CRLF. Errors leave outputs unchanged. */
    UmiStatus UmiEditorTextPositionIndexResolve(const UmiEditorTextPositionIndex *index,
                                                UmiEditorTextPosition position, size_t *out_byte_offset);
    UmiStatus UmiEditorTextPositionIndexAt(const UmiEditorTextPositionIndex *index, size_t byte_offset,
                                           UmiEditorTextPosition *out_position);
    /* Borrow the complete validated text until Destroy. The returned view owns no
 * memory and must not be modified. Revision, save revision and dirty state
 * describe the captured input, never later live edits. Capacity is the owned
 * byte length. Errors leave out_view unchanged. */
    UmiStatus UmiEditorTextPositionIndexView(const UmiEditorTextPositionIndex *index,
                                             UmiEditorTextBufferView *out_view);
#ifdef __cplusplus
}
#endif
#endif
