/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/json_document.h
 * PURPOSE: Validate complete bounded JSON before consumers interpret selected fields.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_JSON_DOCUMENT_H
#define UMICOM_LANGUAGE_RUNTIME_JSON_DOCUMENT_H
#include "umicom/language_runtime/json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_RUNTIME_JSON_MAX_DEPTH 128U
/** Parse at most UMI_LANGUAGE_RUNTIME_JSON_CAPACITY - 1 bytes with at most
 * UMI_LANGUAGE_RUNTIME_JSON_MAX_DEPTH nested containers. Unlike the legacy
 * tokenizer, validate every primitive's JSON grammar and every string's UTF-8,
 * escapes and non-NUL decoded text, including uninterpreted extensions.
 * JSON permits only space, tab, CR and LF outside strings.
 * Duplicate member policy belongs to the consumer; this preserves all tokens.
 * out is unchanged on failure. On success out->json borrows json, which must
 * remain alive and unchanged while tokens are used. Allocate large documents
 * on the heap. The existing tokenizer contract remains available separately. */
UmiStatus UmiLanguageRuntimeJsonParseComplete(const char *json, UmiLanguageRuntimeJsonDocument *out);
#ifdef __cplusplus
}
#endif
#endif
