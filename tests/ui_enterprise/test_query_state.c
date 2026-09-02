/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_query_state.c
 *
 * PURPOSE:
 *   Exercise the query state enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/query_state.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntQueryState q;umi_ui_ent_query_state_init(&q,25U);umi_ui_ent_query_state_set_page(&q,3U);umi_ui_ent_query_state_set_rules(&q,2U,1U);/* Apply this branch only when its contract condition is satisfied. */ if(q.page!=0U||q.sort_count!=2U||q.revision<3U)return 1;puts("ok");return 0;}
