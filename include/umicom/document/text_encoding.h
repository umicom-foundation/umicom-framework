/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/text_encoding.h
 *
 * PURPOSE:
 *   Define loss-aware UTF-8/UTF-16 detection, validation, decode and encode services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_TEXT_ENCODING_H
#define UMICOM_DOCUMENT_TEXT_ENCODING_H

#include "umicom/base/status.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Check that document utf8 satisfies its contract before another service relies on it.
 */
int umi_document_utf8_validate(const unsigned char *bytes,
                               size_t byte_count,
                               size_t *out_error_offset);
/**
 * Provide the document encoding detect operation used by this module and its client
 * applications.
 */
UmiDocumentTextEncoding umi_document_encoding_detect(
    const unsigned char *bytes,
    size_t byte_count,
    int *out_had_bom,
    int *out_binary);
/**
 * Provide the document decode text operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_decode_text(const unsigned char *bytes,
                                   size_t byte_count,
                                   UmiDocumentTextEncoding fallback,
                                   char **out_utf8,
                                   size_t *out_length,
                                   UmiDocumentTextEncoding *out_detected,
                                   int *out_had_bom);
/**
 * Provide the document encode text operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_encode_text(const char *utf8,
                                   size_t utf8_length,
                                   UmiDocumentTextEncoding encoding,
                                   int include_bom,
                                   unsigned char **out_bytes,
                                   size_t *out_byte_count);
/**
 * Provide the document encoding free operation used by this module and its client
 * applications.
 */
void umi_document_encoding_free(void *memory);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_TEXT_ENCODING_H */
