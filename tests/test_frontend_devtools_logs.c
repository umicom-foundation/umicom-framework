/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_frontend_devtools_logs.c
 *
 * PURPOSE:
 *   Implement the test frontend devtools logs behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/frontend/frontend_studio.h"
int main(void){UmiFrontendNetworkLog n;UmiFrontendNetworkEntry e={0},out;UmiFrontendConsoleLog c;UmiFrontendConsoleEntry m={0};(void)strcpy(e.method,"GET");(void)strcpy(e.url,"http://127.0.0.1/app.js");e.status=200;e.kind=UMI_FRONTEND_RESOURCE_SCRIPT;(void)strcpy(m.source,"web/app.js");(void)strcpy(m.message,"ready");m.level=UMI_FRONTEND_CONSOLE_INFO;assert(umi_frontend_network_log_init(&n)==UMI_STATUS_OK);assert(umi_frontend_network_log_append(&n,&e)==UMI_STATUS_OK);assert(umi_frontend_network_log_at(&n,0U,&out)==UMI_STATUS_OK&&out.sequence==1U);assert(umi_frontend_console_log_init(&c)==UMI_STATUS_OK);assert(umi_frontend_console_log_append(&c,&m)==UMI_STATUS_OK);assert(umi_frontend_console_log_count_level(&c,UMI_FRONTEND_CONSOLE_INFO)==1U);return 0;}
