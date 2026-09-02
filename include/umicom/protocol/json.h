/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/json.h
 *
 * PURPOSE:
 *   Provide small dependency-free JSON string escaping and top-level field extraction for protocol messages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_JSON_H
#define UMICOM_PROTOCOL_JSON_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the json escape operation used by this module and its client applications.
 */
UmiStatus umi_json_escape(const char *text,
                          char *out_text,
                          size_t capacity);
/**
 * Provide the json get string operation used by this module and its client applications.
 */
UmiStatus umi_json_get_string(const char *json,
                              const char *field,
                              char *out_text,
                              size_t capacity);
/**
 * Provide the json get integer operation used by this module and its client applications.
 */
UmiStatus umi_json_get_integer(const char *json,
                               const char *field,
                               int64_t *out_value);
/**
 * Provide the json get boolean operation used by this module and its client applications.
 */
UmiStatus umi_json_get_boolean(const char *json,
                               const char *field,
                               int *out_value);
/**
 * Provide the json get object operation used by this module and its client applications.
 */
UmiStatus umi_json_get_object(const char *json,
                              const char *field,
                              char *out_json,
                              size_t capacity);
/**
 * Provide the json is object operation used by this module and its client applications.
 */
int umi_json_is_object(const char *json);

#ifdef __cplusplus
}
#endif

#endif
