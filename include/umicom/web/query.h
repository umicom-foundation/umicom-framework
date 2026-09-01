/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/query.h
 *
 * PURPOSE:
 *   Parse URL query parameters into a bounded deterministic collection.
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

#ifndef UMICOM_WEB_QUERY_H
#define UMICOM_WEB_QUERY_H
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebQueryItem { char name[128]; char value[512]; } UmiWebQueryItem;
typedef struct UmiWebQuery { UmiWebQueryItem items[UMI_WEB_MAX_QUERY_ITEMS]; size_t count; } UmiWebQuery;
UmiStatus umi_web_query_parse(const char *text,UmiWebQuery *out_query);
const char *umi_web_query_get(const UmiWebQuery *query,const char *name);
#ifdef __cplusplus
}
#endif
#endif
