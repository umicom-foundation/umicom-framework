/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_router.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <string.h>
static UmiStatus hello(const UmiWebRequest *req,UmiWebResponse *res,void *data){(void)req;(void)data;return umi_web_response_set_text(res,200,"text/plain","hello");}
int main(void){UmiWebRouter *router=NULL;UmiWebRoute route;UmiWebRequest req;UmiWebResponse res;assert(umi_web_router_create(&router)==UMI_STATUS_OK);assert(umi_web_route_init(&route,UMI_HTTP_METHOD_GET,"/hello",hello,NULL)==UMI_STATUS_OK);assert(umi_web_router_add(router,&route)==UMI_STATUS_OK);umi_web_request_init(&req);req.method=UMI_HTTP_METHOD_GET;assert(umi_web_request_set_target(&req,"/hello")==UMI_STATUS_OK);assert(umi_web_router_dispatch(router,&req,&res)==UMI_STATUS_OK);assert(strcmp(res.body,"hello")==0);umi_web_router_destroy(router);return 0;}
