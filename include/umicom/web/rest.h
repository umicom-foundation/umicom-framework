/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/rest.h
 *
 * PURPOSE:
 *   Provide convenience helpers for REST route registration and JSON responses.
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

#ifndef UMICOM_WEB_REST_H
#define UMICOM_WEB_REST_H
#include "umicom/web/router.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_web_rest_register(UmiWebRouter *router,UmiHttpMethod method,const char *path,UmiWebHandler handler,void *user_data);
UmiStatus umi_web_rest_json(UmiWebResponse *response,int status,const char *json);
#ifdef __cplusplus
}
#endif
#endif
