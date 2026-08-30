/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_virtual_range.c
 *
 * PURPOSE:
 *   Exercise the virtual range enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/virtual_range.h"
#include <stdio.h>
int main(void){UmiUiEntSpan a={2U,5U},b={5U,4U},i=umi_ui_ent_virtual_range_intersection(a,b),u=umi_ui_ent_virtual_range_union(a,b);if(i.first!=5U||i.count!=2U||u.first!=2U||u.count!=7U)return 1;puts("ok");return 0;}
