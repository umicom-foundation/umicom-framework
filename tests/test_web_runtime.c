/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_runtime.c
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
static UmiStatus health(const UmiWebRequest *req,UmiWebResponse *res,void *data){(void)req;(void)data;return umi_web_rest_json(res,200,"{\"status\":\"ok\"}");}
int main(void){UmiWebService *s=NULL;char out[2048];size_t n=0U;const char *wire="GET /health HTTP/1.1\r\nHost: local\r\n\r\n";assert(umi_web_service_create(&s)==UMI_STATUS_OK);assert(umi_web_rest_register(umi_web_service_router(s),UMI_HTTP_METHOD_GET,"/health",health,NULL)==UMI_STATUS_OK);assert(umi_web_runtime_process(s,wire,strlen(wire),out,sizeof(out),&n)==UMI_STATUS_OK);assert(n>0U&&strstr(out,"status")!=NULL);umi_web_service_destroy(s);return 0;}
