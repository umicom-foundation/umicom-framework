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
/**
 * Represent the web service data shared with callers of this public contract.
 */
typedef struct UmiWebService UmiWebService;
/**
 * Initialise web service from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_web_service_create(UmiWebService **out_service);
/**
 * Release or reset state held by web service so the same storage can be reused safely.
 */
void umi_web_service_destroy(UmiWebService *service);
/**
 * Provide the web service router operation used by this module and its client
 * applications.
 */
UmiWebRouter *umi_web_service_router(UmiWebService *service);
/**
 * Provide the web service sessions operation used by this module and its client
 * applications.
 */
UmiWebSessionStore *umi_web_service_sessions(UmiWebService *service);
/**
 * Provide the web service origins operation used by this module and its client
 * applications.
 */
UmiWebOriginPolicy *umi_web_service_origins(UmiWebService *service);
/**
 * Provide the web service endpoints operation used by this module and its client
 * applications.
 */
UmiWebEndpointRegistry *umi_web_service_endpoints(UmiWebService *service);
/**
 * Provide the web service metrics operation used by this module and its client
 * applications.
 */
UmiWebMetrics *umi_web_service_metrics(UmiWebService *service);
/**
 * Perform web service through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_web_service_handle(UmiWebService *service,const UmiWebRequest *request,UmiWebResponse *response);
#ifdef __cplusplus
}
#endif
#endif
