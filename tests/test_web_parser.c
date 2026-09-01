/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_parser.c
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
int main(void){const char *wire="POST /api?q=1 HTTP/1.1\r\nHost: local\r\nContent-Type: text/plain\r\n\r\nabc";UmiWebRequest r;assert(umi_web_parse_request(wire,strlen(wire),&r)==UMI_STATUS_OK);assert(r.method==UMI_HTTP_METHOD_POST);assert(strcmp(r.path,"/api")==0);assert(strcmp(r.body,"abc")==0);return 0;}
