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

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_URI_H */
