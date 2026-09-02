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
/**
 * Represent the web query item data shared with callers of this public contract.
 */
typedef struct UmiWebQueryItem { char name[128]; char value[512]; } UmiWebQueryItem;
/**
 * Represent the web query data shared with callers of this public contract.
 */
typedef struct UmiWebQuery { UmiWebQueryItem items[UMI_WEB_MAX_QUERY_ITEMS]; size_t count; } UmiWebQuery;
/**
 * Read web query into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_web_query_parse(const char *text,UmiWebQuery *out_query);
/**
 * Provide the web query get operation used by this module and its client applications.
 */
const char *umi_web_query_get(const UmiWebQuery *query,const char *name);
#ifdef __cplusplus
}
#endif
#endif
