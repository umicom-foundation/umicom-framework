/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/exception_filter_json_internal.h
 * PURPOSE: Share strict field decoding between exception metadata and acknowledgements.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_EXCEPTION_FILTER_JSON_INTERNAL_H
#define UMICOM_DEBUG_EXCEPTION_FILTER_JSON_INTERNAL_H
#include "umicom/debug_runtime/exception_filters.h"
#include "umicom/language_runtime/json_text.h"
UmiStatus DebugExceptionJsonObject(const UmiLanguageRuntimeJsonDocument *document, int object);
UmiStatus DebugExceptionJsonString(const UmiLanguageRuntimeJsonDocument *document, int object,
                                   const char *key, int required, char *out, size_t capacity);
UmiStatus DebugExceptionJsonBoolean(const UmiLanguageRuntimeJsonDocument *document, int object,
                                    const char *key, int required, int *out);
UmiStatus DebugExceptionJsonResponse(const UmiLanguageRuntimeJsonDocument *document,
                                     const char *command, int body_optional, int *body);
#endif
