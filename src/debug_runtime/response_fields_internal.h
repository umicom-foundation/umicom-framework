/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/response_fields_internal.h
 * PURPOSE: Share strict debugger response-field validation across configurable breakpoint families.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RESPONSE_FIELDS_INTERNAL_H
#define UMICOM_DEBUG_RESPONSE_FIELDS_INTERNAL_H
#include "umicom/base/status.h"
#include "umicom/language_runtime/json_text.h"
/* Find a validated object field by its decoded UTF-8 key, including JSON escapes. */
int DebugResponseJsonField(const UmiLanguageRuntimeJsonDocument *document, int object,
                           const char *key);
UmiStatus DebugResponseJsonObject(const UmiLanguageRuntimeJsonDocument *document, int object);
UmiStatus DebugResponseJsonString(const UmiLanguageRuntimeJsonDocument *document, int object,
                                  const char *key, int required, char *out, size_t capacity);
UmiStatus DebugResponseJsonBoolean(const UmiLanguageRuntimeJsonDocument *document, int object,
                                   const char *key, int required, int *out);
UmiStatus DebugResponseJsonResponse(const UmiLanguageRuntimeJsonDocument *document,
                                    const char *command, int body_optional, int *body);
#endif
