/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_pagination.c
 *
 * PURPOSE:
 *   Exercise the pagination enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/pagination.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntPagination p={2U,25U,63U};UmiUiEntSpan s=umi_ui_ent_pagination_span(&p);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_ui_ent_pagination_page_count(&p)!=3U||s.first!=50U||s.count!=13U)return 1;puts("ok");return 0;}
