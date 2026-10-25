/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/newline.h
 *
 * PURPOSE:
 *   Prepare line endings and literal leading indentation without owning document state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_NEWLINE_H
#define UMICOM_EDITOR_NEWLINE_H
#include "umicom/base/status.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_NEWLINE_MAXIMUM_SOURCE (8U * 1024U * 1024U)
/** Build a separately owned insertion token for a UTF-8 selection.
 * cursor is the lower selection endpoint, or the caret when selection is zero.
 * Validate the whole source and both endpoints, including CRLF boundaries.
 * Use this line's ending, otherwise the previous line's ending, otherwise LF.
 * inherit_indentation is exactly zero or one. One copies only literal spaces
 * and tabs before cursor in this line's leading whitespace; it never guesses
 * syntax, changes tab widths, or converts mixed indentation.
 * LF and CRLF are supported. Bare CR returns NOT_IMPLEMENTED. Malformed UTF-8,
 * embedded zeros and invalid ranges refuse the edit. Input and result stay
 * within MAXIMUM_SOURCE bytes. Outputs stay unchanged on failure.
 * Release a successful token with UmiEditorNewlineFree. No history or I/O. */
UmiStatus UmiEditorNewlineCreate(const char *source, size_t source_bytes, size_t cursor,
                                 size_t selection, int inherit_indentation, char **out_text,
                                 size_t *out_bytes);
void UmiEditorNewlineFree(char *text);
#ifdef __cplusplus
}
#endif
#endif
