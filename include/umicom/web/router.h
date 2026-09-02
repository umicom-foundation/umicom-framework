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

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_ROUTER_H
#define UMICOM_WEB_ROUTER_H
#include "umicom/web/route.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web router data shared with callers of this public contract.
 */
typedef struct UmiWebRouter UmiWebRouter;
/**
 * Initialise web router from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_web_router_create(UmiWebRouter **out_router);
/**
 * Release or reset state held by web router so the same storage can be reused safely.
 */
void umi_web_router_destroy(UmiWebRouter *router);
/**
 * Add web router only after its inputs and available capacity have been checked.
 */
UmiStatus umi_web_router_add(UmiWebRouter *router,const UmiWebRoute *route);
/**
 * Perform web router through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_web_router_dispatch(UmiWebRouter *router,const UmiWebRequest *request,UmiWebResponse *response);
/**
 * Return the number of records represented by web router without changing their state.
 */
size_t umi_web_router_count(const UmiWebRouter *router);
#ifdef __cplusplus
}
#endif
#endif
