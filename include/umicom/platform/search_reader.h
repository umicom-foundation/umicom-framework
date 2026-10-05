/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/search_reader.h
 * PURPOSE: Allow higher-level document services to supply decoded search text without a platform-to-document dependency.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_SEARCH_READER_H
#define UMICOM_PLATFORM_SEARCH_READER_H
#include "umicom/platform/search.h"
#include "umicom/platform/search_filter.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiSearchFileReader
    {
        void *context;
        size_t maximum_serialized_bytes;
        /* Called on the searching thread with the requested decoded-text bound.
     * Return an independently owned buffer and its exact byte length, with no
     * embedded NUL. Text must be valid UTF-8 with LF line endings so byte
     * columns and previews can be used by document editors. Empty
     * text may use NULL. Failure must clear outputs. NOT_IMPLEMENTED skips an
     * unsupported/binary file; CAPACITY_EXCEEDED counts an oversized file.
     * Other failures stop the search. Honour cancellation between bounded work. */
        UmiStatus (*read)(void *context, const char *path, size_t maximum_text_bytes,
                          const UmiCancellationToken *cancel, unsigned char **out_text, size_t *out_bytes);
        /* Pair every non-NULL returned allocation with this release callback,
     * including a reader which returned failure after allocating output. */
        void (*release)(void *context, void *text);
    } UmiSearchFileReader;
    /* Copy the reader descriptor for this synchronous search. Its context must
 * remain alive until return. Filter/index coherence, cancellation, bounded
 * results and literal matching use the original shared engine. With a custom
 * reader, maximum_file_size bounds returned text, while the reader's serialized
 * bound gates cached file sizes. NULL retains the original raw-byte search.
 * Reader callbacks must not destroy the index or borrowed cancellation token.
 * Partial streaming matches must be discarded when this operation fails. */
    UmiStatus UmiSearchFileIndexWithReader(const UmiFileIndex *index, const UmiSearchRequest *request,
                                           const UmiSearchOptions *options, const UmiSearchPathFilter *filter,
                                           const UmiSearchFileReader *reader, UmiSearchMatchSink sink,
                                           void *user_data, UmiSearchStats *out_stats);
#ifdef __cplusplus
}
#endif
#endif
