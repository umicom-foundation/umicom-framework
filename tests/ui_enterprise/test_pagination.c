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
int main(void){UmiUiEntPagination p={2U,25U,63U};UmiUiEntSpan s=umi_ui_ent_pagination_span(&p);if(umi_ui_ent_pagination_page_count(&p)!=3U||s.first!=50U||s.count!=13U)return 1;puts("ok");return 0;}
