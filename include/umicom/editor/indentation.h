/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/indentation.h
 *
 * PURPOSE:
 *   Share validated indentation units and deterministic tab-stop calculation across document and
 * native editor services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_INDENTATION_H
#define UMICOM_EDITOR_INDENTATION_H
#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_INDENTATION_MAXIMUM_WIDTH 16U
typedef struct UmiEditorIndentation {
  uint32_t tab_width;
  int insert_spaces;
} UmiEditorIndentation;
/* Defaults are four columns and spaces. Validation accepts width 1..16 and
 * a strict boolean. Failures leave output values and buffers unchanged. */
UmiStatus UmiEditorIndentationDefault(UmiEditorIndentation *out_options);
UmiStatus UmiEditorIndentationValidate(const UmiEditorIndentation *options);
/* Write one complete indent unit plus its terminator. No allocation. */
UmiStatus UmiEditorIndentationUnit(const UmiEditorIndentation *options, char *output,
                                   size_t capacity, size_t *out_bytes);
/* Write the next tab-stop insertion at a valid UTF-8 byte offset. Tabs advance
 * to the next configured stop; other Unicode scalars count as one column.
 * This deliberately does not claim terminal-cell/grapheme display width.
 * The scanned prefix is validated, including CRLF boundaries. No source or
 * history mutation occurs. When spaces are disabled, insert one literal tab. */
UmiStatus UmiEditorIndentationAt(const UmiEditorIndentation *options, const char *text,
                                 size_t bytes, size_t offset, char *output, size_t capacity,
                                 size_t *out_bytes);
#ifdef __cplusplus
}
#endif
#endif
