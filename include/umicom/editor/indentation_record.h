/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/indentation_record.h
 *
 * PURPOSE:
 *   Encode bounded, versioned editor indentation preferences without filesystem or toolkit
 * dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_INDENTATION_RECORD_H
#define UMICOM_EDITOR_INDENTATION_RECORD_H
#include "umicom/editor/indentation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_INDENTATION_RECORD_MAXIMUM_BYTES 128U
/** Canonical UTF-8/ASCII record with LF endings. capacity includes trailing
 * NUL; out_size excludes it. Both outputs remain unchanged on failure. */
UmiStatus UmiEditorIndentationEncode(const UmiEditorIndentation *options, char *out_text,
                                     size_t capacity, size_t *out_size);
/** Accept the three ordered fields in format version 1, with LF or CRLF
 * endings and an optional final newline. Reject NUL, unknown/duplicate fields,
 * extra lines, invalid widths and unsupported versions. Output is unchanged
 * on failure. Caller owns the bounded input for the synchronous operation. */
UmiStatus UmiEditorIndentationDecode(const void *bytes, size_t size,
                                     UmiEditorIndentation *out_options);
#ifdef __cplusplus
}
#endif
#endif
