/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_types.c
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
int main(void){char text[8];assert(umi_http_method_from_text("GET")==UMI_HTTP_METHOD_GET);assert(strcmp(umi_http_status_reason(404),"Not Found")==0);assert(umi_web_copy_text(text,sizeof(text),"ok")==UMI_STATUS_OK);return 0;}
