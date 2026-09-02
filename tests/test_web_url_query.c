/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_url_query.c
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
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiWebUrl u;UmiWebQuery q;assert(umi_web_url_parse("https://example.com:8443/a?x=1",&u)==UMI_STATUS_OK);assert(u.port==8443U);assert(strcmp(u.path,"/a")==0);assert(umi_web_query_parse("x=hello+world&n=2",&q)==UMI_STATUS_OK);assert(strcmp(umi_web_query_get(&q,"x"),"hello world")==0);return 0;}
