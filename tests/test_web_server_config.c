/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_server_config.c
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
int main(void){UmiWebServerConfig c=umi_web_server_config_default();UmiWebServerState s;assert(umi_web_server_config_validate(&c)==UMI_STATUS_OK);umi_web_server_state_init(&s);assert(s.phase==UMI_WEB_SERVER_STOPPED);return 0;}
