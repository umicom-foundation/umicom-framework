/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_selection_range.c
 *
 * PURPOSE:
 *   Exercise the selection range enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/selection_range.h"
#include <stdio.h>
int main(void){UmiUiEntSelectionRange r={4U,2U,3U,1U};r=umi_ui_ent_selection_range_normalize(r);if(r.row_first!=2U||r.column_first!=1U||!umi_ui_ent_selection_range_contains(r,3U,2U))return 1;puts("ok");return 0;}
