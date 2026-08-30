/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/router.h
 *
 * PURPOSE:
 *   Register and dispatch bounded HTTP routes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_ROUTER_H
#define UMICOM_WEB_ROUTER_H
#include "umicom/web/route.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebRouter UmiWebRouter;
UmiStatus umi_web_router_create(UmiWebRouter **out_router);
void umi_web_router_destroy(UmiWebRouter *router);
UmiStatus umi_web_router_add(UmiWebRouter *router,const UmiWebRoute *route);
UmiStatus umi_web_router_dispatch(UmiWebRouter *router,const UmiWebRequest *request,UmiWebResponse *response);
size_t umi_web_router_count(const UmiWebRouter *router);
#ifdef __cplusplus
}
#endif
#endif
