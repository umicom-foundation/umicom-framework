/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/parser.h
 *
 * PURPOSE:
 *   Parse an HTTP/1.x request message into the toolkit-neutral request model.
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

#ifndef UMICOM_WEB_PARSER_H
#define UMICOM_WEB_PARSER_H
#include "umicom/web/request.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_web_parse_request(const char *text,size_t length,UmiWebRequest *out_request);
#ifdef __cplusplus
}
#endif
#endif
