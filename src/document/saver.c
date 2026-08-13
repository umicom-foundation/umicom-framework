/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/saver.c
 *
 * PURPOSE:
 *   Apply line-ending/final-newline policy, encode text and persist it atomically.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/saver.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/document/fingerprint.h"
#include "umicom/document/line_endings.h"
#include "umicom/document/text_encoding.h"

UmiDocumentSaveOptions umi_document_save_options_default(void)
{
#ifdef _WIN32
    UmiDocumentLineEnding ending = UMI_DOCUMENT_LINE_ENDING_CRLF;
#else
    UmiDocumentLineEnding ending = UMI_DOCUMENT_LINE_ENDING_LF;
#endif
    return (UmiDocumentSaveOptions){
        UMI_DOCUMENT_ENCODING_UTF8,
        ending,
        0,
        0,
        1
    };
}

UmiStatus umi_document_saver_write(const UmiDocumentProviderV1 *provider,
                                   const char *resource,
                                   const char *utf8_text,
                                   size_t text_length,
                                   const UmiDocumentSaveOptions *options,
                                   UmiDocumentSaveResult *out_result)
{
    UmiDocumentSaveOptions effective = umi_document_save_options_default();
    char *normalised = NULL;
    size_t normalised_length = 0U;
    unsigned char *encoded = NULL;
    size_t encoded_length = 0U;
    UmiStatus status;
    if (provider == NULL || resource == NULL ||
        (utf8_text == NULL && text_length > 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (options != NULL) effective = *options;
    if (effective.line_ending < UMI_DOCUMENT_LINE_ENDING_LF ||
        effective.line_ending > UMI_DOCUMENT_LINE_ENDING_CR) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_document_line_endings_normalise(
        utf8_text, text_length, effective.line_ending,
        effective.ensure_final_newline, &normalised, &normalised_length);
    if (status == UMI_STATUS_OK) {
        status = umi_document_encode_text(normalised, normalised_length,
                                          effective.encoding,
                                          effective.include_bom,
                                          &encoded, &encoded_length);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_document_provider_write(provider, resource, encoded,
                                             encoded_length,
                                             effective.atomic_replace);
    }
    if (status == UMI_STATUS_OK && out_result != NULL) {
        (void)memset(out_result, 0, sizeof(*out_result));
        out_result->bytes_written = encoded_length;
        out_result->fingerprint.content_hash = umi_document_hash_bytes(encoded,
                                                                        encoded_length);
        out_result->fingerprint.byte_count = encoded_length;
        out_result->fingerprint.valid = 1;
        out_result->atomic_replace_used = effective.atomic_replace;
    }
    free(normalised);
    umi_document_encoding_free(encoded);
    return status;
}
