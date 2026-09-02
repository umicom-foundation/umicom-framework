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
/**
 * Represent the web middleware chain data shared with callers of this public contract.
 */
typedef struct UmiWebMiddlewareChain UmiWebMiddlewareChain;
/**
 * Initialise web middleware chain from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_web_middleware_chain_create(UmiWebMiddlewareChain **out_chain);
/**
 * Release or reset state held by web middleware chain so the same storage can be reused
 * safely.
 */
void umi_web_middleware_chain_destroy(UmiWebMiddlewareChain *chain);
/**
 * Add web middleware chain only after its inputs and available capacity have been checked.
 */
UmiStatus umi_web_middleware_chain_add(UmiWebMiddlewareChain *chain,UmiWebMiddleware middleware,void *user_data);
/**
 * Perform web middleware chain through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_web_middleware_chain_run(const UmiWebMiddlewareChain *chain,const UmiWebRequest *request,UmiWebResponse *response);
/**
 * Return the number of records represented by web middleware chain without changing their
 * state.
 */
size_t umi_web_middleware_chain_count(const UmiWebMiddlewareChain *chain);
#ifdef __cplusplus
}
#endif
#endif
