/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/middleware.h
 *
 * PURPOSE:
 *   Run reusable request middleware before route dispatch.
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

#ifndef UMICOM_WEB_MIDDLEWARE_H
#define UMICOM_WEB_MIDDLEWARE_H
#include "umicom/web/request.h"
#include "umicom/web/response.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef UmiStatus (*UmiWebMiddleware)(const UmiWebRequest *request,UmiWebResponse *response,void *user_data);
typedef struct UmiWebMiddlewareChain UmiWebMiddlewareChain;
UmiStatus umi_web_middleware_chain_create(UmiWebMiddlewareChain **out_chain);
void umi_web_middleware_chain_destroy(UmiWebMiddlewareChain *chain);
UmiStatus umi_web_middleware_chain_add(UmiWebMiddlewareChain *chain,UmiWebMiddleware middleware,void *user_data);
UmiStatus umi_web_middleware_chain_run(const UmiWebMiddlewareChain *chain,const UmiWebRequest *request,UmiWebResponse *response);
size_t umi_web_middleware_chain_count(const UmiWebMiddlewareChain *chain);
#ifdef __cplusplus
}
#endif
#endif
