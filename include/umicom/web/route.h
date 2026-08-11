/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/route.h
 *
 * PURPOSE:
 *   Describe one method/path route and its request handler.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_ROUTE_H
#define UMICOM_WEB_ROUTE_H
#include "umicom/web/request.h"
#include "umicom/web/response.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef UmiStatus (*UmiWebHandler)(const UmiWebRequest *request,UmiWebResponse *response,void *user_data);
typedef struct UmiWebRoute { UmiHttpMethod method; char pattern[UMI_WEB_PATH_CAPACITY]; UmiWebHandler handler; void *user_data; } UmiWebRoute;
UmiStatus umi_web_route_init(UmiWebRoute *route,UmiHttpMethod method,const char *pattern,UmiWebHandler handler,void *user_data);
int umi_web_route_matches(const UmiWebRoute *route,const UmiWebRequest *request);
#ifdef __cplusplus
}
#endif
#endif
