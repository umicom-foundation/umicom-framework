/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/search_range.h
 *
 * PURPOSE:
 *   Replace non-overlapping literal matches within an explicit byte range while retaining
 * whole-source word boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_SEARCH_RANGE_H
#define UMICOM_EDITOR_SEARCH_RANGE_H
#include "umicom/editor/search_engine.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Byte-oriented search. [start,start+span) must fit inside the supplied source.
 * Only complete matches inside that range are replaced. Whole-word boundaries
 * still examine adjacent bytes in the FULL source, not an isolated substring.
 * Zero span means no matches, never whole-document replacement. NULL options
 * use exact case; case_mode and whole_word are used, other fields are ignored.
 * All original non-overlapping matches are considered without a result cap.
 * Output size includes unchanged prefix/suffix but excludes the final zero.
 * Failures preserve supplied output values. Input storage is borrowed. */
UmiStatus UmiEditorSearchReplaceRangeSize(const char *text, size_t text_bytes, size_t start,
                                          size_t span, const char *needle, size_t needle_bytes,
                                          size_t replacement_bytes,
                                          const UmiEditorSearchOptions *options, size_t *out_bytes,
                                          size_t *out_count);
/* Produce the complete source with that range transformed. Capacity includes
 * the terminating zero. Output must not overlap text, needle or replacement;
 * overlap is rejected before writes. NUL bytes are allowed in these generic
 * byte buffers; document clients separately enforce their UTF-8 text policy. */
UmiStatus UmiEditorSearchReplaceRange(const char *text, size_t text_bytes, size_t start,
                                      size_t span, const char *needle, size_t needle_bytes,
                                      const char *replacement, size_t replacement_bytes,
                                      const UmiEditorSearchOptions *options, char *out_text,
                                      size_t capacity, size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
