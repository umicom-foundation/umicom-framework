/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/file_search.c
 * PURPOSE: Supply Unicode decoding above the platform search engine without duplicating matching or session ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/file_search.h"
#include "umicom/document/coordinator.h"
#include "umicom/document/text_encoding.h"
#include "umicom/document/line_endings.h"
#include "umicom/platform/input_file.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus DocumentSearchRead(void *context, const char *path, size_t maximum_text_bytes,
                                    const UmiCancellationToken *cancel, unsigned char **out,
                                    size_t *out_bytes)
{
    (void)context;
    if (out == NULL || out_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    *out_bytes = 0U;
    if (maximum_text_bytes == 0U || maximum_text_bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    unsigned char *raw = NULL;
    size_t raw_bytes = 0U;
    /* A handle-bounded read also supports ordinary Unicode Windows paths.
     * The encoded envelope includes BOM and CRLF expansion; the independent
     * decoded bound below still prevents a large editable draft. */
    size_t maximum_encoded = 4U * maximum_text_bytes + 3U;
    UmiStatus status = UmiInputFileRead(path, maximum_encoded, &raw, &raw_bytes);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    char *decoded = NULL, *normalized = NULL;
    size_t decoded_bytes = 0U, normalized_bytes = 0U;
    UmiDocumentTextEncoding encoding;
    int had_bom = 0, binary = 0;
    if (status == UMI_STATUS_OK)
    {
        encoding = umi_document_encoding_detect(raw, raw_bytes, &had_bom, &binary);
        if (binary)
            status = UMI_STATUS_NOT_IMPLEMENTED;
        else
        {
            status = umi_document_decode_text(raw, raw_bytes, UMI_DOCUMENT_ENCODING_UTF8, &decoded,
                                              &decoded_bytes, &encoding, &had_bom);
            /* Inputs above are valid; decoder refusal describes malformed file
             * content, not a bad search request. Count it and search on. */
            if (status == UMI_STATUS_INVALID_ARGUMENT || status == UMI_STATUS_PARSE_ERROR)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    if (status == UMI_STATUS_OK && memchr(decoded, '\0', decoded_bytes) != NULL)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_document_line_endings_normalise(decoded, decoded_bytes, UMI_DOCUMENT_LINE_ENDING_LF, 0,
                                                     &normalized, &normalized_bytes);
    if (status == UMI_STATUS_OK && normalized_bytes > maximum_text_bytes)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    UmiInputFileFree(raw);
    umi_document_encoding_free(decoded);
    if (status != UMI_STATUS_OK)
    {
        umi_document_line_endings_free(normalized);
        return status;
    }
    *out = (unsigned char *)normalized;
    *out_bytes = normalized_bytes;
    return UMI_STATUS_OK;
}
static void DocumentSearchRelease(void *context, void *text)
{
    (void)context;
    umi_document_line_endings_free(text);
}
static UmiSearchFileReader DocumentSearchReader(void)
{
    UmiSearchFileReader reader = {NULL, UMI_DOCUMENT_COORDINATOR_MAXIMUM_FILE_BYTES, DocumentSearchRead,
                                  DocumentSearchRelease};
    return reader;
}
UmiStatus UmiDocumentSearchFileIndex(const UmiFileIndex *index, const UmiSearchRequest *request,
                                     const UmiSearchOptions *options, const UmiSearchPathFilter *filter,
                                     UmiSearchMatchSink sink, void *user_data, UmiSearchStats *out_stats)
{
    if (out_stats != NULL)
        memset(out_stats, 0, sizeof(*out_stats));
    if (request == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (request->maximum_file_size > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiSearchFileReader reader = DocumentSearchReader();
    return UmiSearchFileIndexWithReader(index, request, options, filter, &reader, sink, user_data, out_stats);
}
UmiStatus UmiDocumentFileSearchCreate(const UmiFileIndex *index, UmiFileSearchSession **out_session)
{
    UmiSearchFileReader reader = DocumentSearchReader();
    return UmiFileSearchCreateWithReader(index, &reader, out_session);
}
