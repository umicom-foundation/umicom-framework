/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/json.h
 *
 * PURPOSE:
 *   Provide small dependency-free JSON string escaping and top-level field extraction for protocol messages.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_JSON_H
#define UMICOM_PROTOCOL_JSON_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_json_escape(const char *text,
                          char *out_text,
                          size_t capacity);
UmiStatus umi_json_get_string(const char *json,
                              const char *field,
                              char *out_text,
                              size_t capacity);
UmiStatus umi_json_get_integer(const char *json,
                               const char *field,
                               int64_t *out_value);
UmiStatus umi_json_get_boolean(const char *json,
                               const char *field,
                               int *out_value);
UmiStatus umi_json_get_object(const char *json,
                              const char *field,
                              char *out_json,
                              size_t capacity);
int umi_json_is_object(const char *json);

#ifdef __cplusplus
}
#endif

#endif
