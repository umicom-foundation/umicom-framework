/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_request.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <string.h>
int main(void){UmiWebRequest r;umi_web_request_init(&r);assert(umi_web_request_set_target(&r,"/items?q=one")==UMI_STATUS_OK);assert(strcmp(r.path,"/items")==0);assert(strcmp(r.query,"q=one")==0);assert(umi_web_header_set(&r.headers[0],"Content-Type","text/plain")==UMI_STATUS_OK);r.header_count=1U;assert(strcmp(umi_web_request_header(&r,"content-type"),"text/plain")==0);return 0;}
