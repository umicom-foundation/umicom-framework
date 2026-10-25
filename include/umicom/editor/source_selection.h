/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/source_selection.h
 *
 * PURPOSE:
 *   Resolve source-word and physical-line selections without editing borrowed UTF-8 text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_SOURCE_SELECTION_H
#define UMICOM_EDITOR_SOURCE_SELECTION_H
#include "umicom/editor/text_position.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiEditorSourceSelectionKind {
  UMI_EDITOR_SELECT_SOURCE_WORD = 0,
  UMI_EDITOR_SELECT_LINE_CONTENT = 1,
  UMI_EDITOR_SELECT_LINE_WITH_ENDING = 2
} UmiEditorSourceSelectionKind;
typedef struct UmiEditorSourceSelection {
  size_t offset, byte_count;
} UmiEditorSourceSelection;
/* Read stable borrowed UTF-8, at most 8 MiB, without allocation or mutation.
 * Caret is a byte offset at a complete scalar/CRLF boundary. Output changes
 * only on success. Malformed UTF-8, embedded NUL and invalid kinds fail.
 * WORD selects contiguous ASCII letters/digits/underscore or non-ASCII bytes.
 * This is a source-token convenience, not Unicode linguistic segmentation.
 * Prefer the word at the caret, then immediately before it; otherwise NOT_FOUND.
 * LINE selects the current physical line. CRLF is one ending; CR and LF also
 * end lines. At end after a final newline the last line is empty. An empty
 * document has an empty line. Existing selections are deliberately not inputs. */
UmiStatus UmiEditorSourceSelectionAt(const UmiEditorTextBufferView *view, size_t caret,
                                     UmiEditorSourceSelectionKind kind,
                                     UmiEditorSourceSelection *out);
#ifdef __cplusplus
}
#endif
#endif
