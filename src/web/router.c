/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/router.c
 *
 * PURPOSE:
 *   Implement route registration and request dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/router.h"
#include <stdlib.h>
#include <string.h>
struct UmiWebRouter{UmiWebRoute routes[UMI_WEB_MAX_ROUTES];size_t count;};
/*
 * Initialise web router from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_web_router_create(UmiWebRouter **out_router){UmiWebRouter *r;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_router==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_router=NULL;r=(UmiWebRouter*)calloc(1U,sizeof(*r));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out_router=r;return UMI_STATUS_OK;}
/* Release or reset state held by web router so the same storage can be reused safely. */
void umi_web_router_destroy(UmiWebRouter *router){free(router);}
/* Add web router only after its inputs and available capacity have been checked. */
UmiStatus umi_web_router_add(UmiWebRouter *router,const UmiWebRoute *route){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(router==NULL||route==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(router->count>=UMI_WEB_MAX_ROUTES)return UMI_STATUS_CAPACITY_EXCEEDED;router->routes[router->count++]=*route;return UMI_STATUS_OK;}
/*
 * Perform web router through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_web_router_dispatch(UmiWebRouter *router,const UmiWebRequest *request,UmiWebResponse *response){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(router==NULL||request==NULL||response==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<router->count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_web_route_matches(&router->routes[i],request))return router->routes[i].handler(request,response,router->routes[i].user_data);}return umi_web_response_set_text(response,UMI_HTTP_STATUS_NOT_FOUND,"text/plain; charset=utf-8","Not Found");}
/* Return the number of records represented by web router without changing their state. */
size_t umi_web_router_count(const UmiWebRouter *router){return router!=NULL?router->count:0U;}
