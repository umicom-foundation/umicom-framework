/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/file_search.h
 * PURPOSE: Search saved Unicode document text with the same decoding and line coordinates as the editor.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_FILE_SEARCH_H
#define UMICOM_DOCUMENT_FILE_SEARCH_H
#include "umicom/platform/search_session.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Search saved UTF-8 (with or without BOM) and BOM-marked UTF-16 LE/BE.
 * Decode and normalize CR/CRLF to LF before matching. Columns and previews
 * describe UTF-8 editor text, not encoded disk-byte offsets. Unsupported or
 * malformed text counts in binary_files_skipped; capacity failures count in
 * oversized_files_skipped. Other file errors stop the search.
 * maximum_file_size bounds decoded text, up to eight MiB; serialized admission
 * allows encoding and newline expansion. Reads require ordinary absolute
 * paths and regular non-link leaves; parent aliases are not a sandbox.
 * Input must describe a trusted workspace. Cancellation is checked between
 * bounded read/decode/normalize phases; a blocking OS read can finish first.
 * Existing filters, index revisions and result limits remain in force. */
    UmiStatus UmiDocumentSearchFileIndex(const UmiFileIndex *index, const UmiSearchRequest *request,
                                         const UmiSearchOptions *options, const UmiSearchPathFilter *filter,
                                         UmiSearchMatchSink sink, void *user_data, UmiSearchStats *out_stats);
    /* Use the same document reader on the existing lazy search worker. The session
 * owns its copied descriptor and has no external reader context. Its usual
 * lifecycle/result APIs apply; destroy it before the borrowed file index. */
    UmiStatus UmiDocumentFileSearchCreate(const UmiFileIndex *index, UmiFileSearchSession **out_session);
#ifdef __cplusplus
}
#endif
#endif
