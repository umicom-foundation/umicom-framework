/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/url.h
 *
 * PURPOSE:
 *   Parse and percent-decode HTTP and HTTPS URLs.
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

#ifndef UMICOM_WEB_URL_H
#define UMICOM_WEB_URL_H
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web url data shared with callers of this public contract.
 */
typedef struct UmiWebUrl { char scheme[16]; char host[256]; uint16_t port; char path[UMI_WEB_PATH_CAPACITY]; char query[UMI_WEB_QUERY_CAPACITY]; } UmiWebUrl;
/**
 * Read web url into validated module state and return a status when input cannot be used.
 */
UmiStatus umi_web_url_parse(const char *text,UmiWebUrl *out_url);
/**
 * Read web url into validated module state and return a status when input cannot be used.
 */
UmiStatus umi_web_url_decode(const char *text,char *out_text,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
