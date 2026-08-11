/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_response.c
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
int main(void){UmiWebResponse r;char wire[1024];size_t n=0U;assert(umi_web_response_set_text(&r,200,"text/plain","hello")==UMI_STATUS_OK);assert(umi_web_response_format(&r,wire,sizeof(wire),&n)==UMI_STATUS_OK);assert(n>0U);assert(strstr(wire,"HTTP/1.1 200 OK")!=NULL);assert(strstr(wire,"hello")!=NULL);return 0;}
