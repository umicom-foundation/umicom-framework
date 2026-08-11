/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_json_streams.c
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
int main(void){char json[256],sse[512];size_t n=0U;UmiWebSseEvent e={7U,"update","ready"};assert(umi_web_json_object1("state","ready",json,sizeof(json))==UMI_STATUS_OK);assert(strstr(json,"ready")!=NULL);assert(umi_web_sse_format(&e,sse,sizeof(sse),&n)==UMI_STATUS_OK);assert(n>0U&&strstr(sse,"event: update")!=NULL);return 0;}
