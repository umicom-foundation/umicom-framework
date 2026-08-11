/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/service.c
 *
 * PURPOSE:
 *   Compose routing and operational web services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/service.h"
#include <stdlib.h>
struct UmiWebService{UmiWebRouter *router;UmiWebMiddlewareChain *middleware;UmiWebSessionStore *sessions;UmiWebOriginPolicy *origins;UmiWebEndpointRegistry *endpoints;UmiWebMetrics metrics;};
UmiStatus umi_web_service_create(UmiWebService **out_service){UmiWebService *s;UmiStatus st;if(out_service==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_service=NULL;s=(UmiWebService*)calloc(1U,sizeof(*s));if(s==NULL)return UMI_STATUS_OUT_OF_MEMORY;st=umi_web_router_create(&s->router);if(st==UMI_STATUS_OK)st=umi_web_middleware_chain_create(&s->middleware);if(st==UMI_STATUS_OK)st=umi_web_session_store_create(&s->sessions);if(st==UMI_STATUS_OK)st=umi_web_origin_policy_create(&s->origins);if(st==UMI_STATUS_OK)st=umi_web_endpoint_registry_create(&s->endpoints);if(st!=UMI_STATUS_OK){umi_web_service_destroy(s);return st;}umi_web_metrics_init(&s->metrics);*out_service=s;return UMI_STATUS_OK;}
void umi_web_service_destroy(UmiWebService *s){if(s==NULL)return;umi_web_endpoint_registry_destroy(s->endpoints);umi_web_origin_policy_destroy(s->origins);umi_web_session_store_destroy(s->sessions);umi_web_middleware_chain_destroy(s->middleware);umi_web_router_destroy(s->router);free(s);}
UmiWebRouter *umi_web_service_router(UmiWebService *s){return s!=NULL?s->router:NULL;}UmiWebSessionStore *umi_web_service_sessions(UmiWebService *s){return s!=NULL?s->sessions:NULL;}UmiWebOriginPolicy *umi_web_service_origins(UmiWebService *s){return s!=NULL?s->origins:NULL;}UmiWebEndpointRegistry *umi_web_service_endpoints(UmiWebService *s){return s!=NULL?s->endpoints:NULL;}UmiWebMetrics *umi_web_service_metrics(UmiWebService *s){return s!=NULL?&s->metrics:NULL;}
UmiStatus umi_web_service_handle(UmiWebService *s,const UmiWebRequest *req,UmiWebResponse *res){UmiStatus st;if(s==NULL||req==NULL||res==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_web_response_init(res);st=umi_web_middleware_chain_run(s->middleware,req,res);if(st==UMI_STATUS_OK)st=umi_web_router_dispatch(s->router,req,res);umi_web_metrics_record(&s->metrics,req->body_length,res->status,res->body_length);return st;}
