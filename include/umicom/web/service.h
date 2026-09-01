/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/service.h
 *
 * PURPOSE:
 *   Compose routing, sessions, origins, endpoint metadata and metrics into one web service.
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

#ifndef UMICOM_WEB_SERVICE_H
#define UMICOM_WEB_SERVICE_H
#include "umicom/web/endpoint.h"
#include "umicom/web/metrics.h"
#include "umicom/web/middleware.h"
#include "umicom/web/origin.h"
#include "umicom/web/router.h"
#include "umicom/web/session.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebService UmiWebService;
UmiStatus umi_web_service_create(UmiWebService **out_service);
void umi_web_service_destroy(UmiWebService *service);
UmiWebRouter *umi_web_service_router(UmiWebService *service);
UmiWebSessionStore *umi_web_service_sessions(UmiWebService *service);
UmiWebOriginPolicy *umi_web_service_origins(UmiWebService *service);
UmiWebEndpointRegistry *umi_web_service_endpoints(UmiWebService *service);
UmiWebMetrics *umi_web_service_metrics(UmiWebService *service);
UmiStatus umi_web_service_handle(UmiWebService *service,const UmiWebRequest *request,UmiWebResponse *response);
#ifdef __cplusplus
}
#endif
#endif
