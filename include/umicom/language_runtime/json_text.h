/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/json_text.h
 * PURPOSE: Decode complete JSON string tokens as bounded UTF-8 text.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_JSON_TEXT_H
#define UMICOM_LANGUAGE_RUNTIME_JSON_TEXT_H
#include "umicom/language_runtime/json.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Decode one parsed string token, including paired UTF-16 surrogate escapes.
 * Reject malformed UTF-8, lone surrogates, raw controls and embedded NUL.
 * Failure leaves out unchanged. Input document/text must remain unchanged
 * during the call; output must not overlap them. Capacity includes the NUL.
 * This supplements the legacy string reader without changing its contract. */
UmiStatus UmiLanguageRuntimeJsonText(const UmiLanguageRuntimeJsonDocument *document,
    int token, char *out, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
