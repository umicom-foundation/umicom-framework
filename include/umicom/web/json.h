/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/json.h
 *
 * PURPOSE:
 *   Provide small JSON escaping helpers for web endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_JSON_H
#define UMICOM_WEB_JSON_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_web_json_escape(const char *text,char *out_text,size_t capacity);
UmiStatus umi_web_json_object1(const char *name,const char *value,char *out_text,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
