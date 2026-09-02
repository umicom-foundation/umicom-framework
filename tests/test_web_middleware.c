/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_middleware.c
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

/*
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
/*
 * Exercise mark and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus mark(const UmiWebRequest *req,UmiWebResponse *res,void *data){int *count=(int*)data;(void)req;(void)res;++*count;return UMI_STATUS_OK;}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiWebMiddlewareChain *c=NULL;UmiWebRequest req;UmiWebResponse res;int count=0;assert(umi_web_middleware_chain_create(&c)==UMI_STATUS_OK);assert(umi_web_middleware_chain_add(c,mark,&count)==UMI_STATUS_OK);umi_web_request_init(&req);umi_web_response_init(&res);assert(umi_web_middleware_chain_run(c,&req,&res)==UMI_STATUS_OK);assert(count==1);umi_web_middleware_chain_destroy(c);return 0;}
