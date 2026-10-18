/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/uri.h
 *
 * PURPOSE:
 *   Define reversible native-path and RFC 8089 file-URI conversion utilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_URI_H
#define UMICOM_DOCUMENT_URI_H

#include "umicom/base/status.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Conversion accepts absolute UTF-8 native paths and the file:// spelling
 * produced by this API. Failures leave output unchanged; in-place conversion
 * is supported. Decoding rejects encoded NUL, malformed UTF-8, relative paths
 * and unescaped URI query/fragment delimiters. Authority-host forms are not
 * translated into remote paths; native UNC paths keep their existing spelling. */
/**
 * Provide the document uri from path operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_uri_from_path(const char *path,
                                     char *out_uri,
                                     size_t capacity);
/**
 * Provide the document uri to path operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_uri_to_path(const char *uri,
                                   char *out_path,
                                   size_t capacity);
/**
 * Provide the document uri normalise operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_uri_normalise(const char *uri,
                                     char *out_uri,
                                     size_t capacity);
/**
 * Provide the document uri is file operation used by this module and its client
 * applications.
 */
int umi_document_uri_is_file(const char *uri);

/**
 * Encode a UTF-8 display name as one untitled URI path segment. No file is
 * created or looked up. Spaces, percent signs and path punctuation are escaped;
 * a failure leaves the caller's output untouched. Plain names keep their
 * existing untitled:///name spelling for compatibility.
 */
UmiStatus umi_document_uri_from_untitled_name(const char *name,
                                             char *out_uri,
                                             size_t capacity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_URI_H */
