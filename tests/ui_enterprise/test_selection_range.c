/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_selection_range.c
 *
 * PURPOSE:
 *   Exercise the selection range enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/selection_range.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntSelectionRange r={4U,2U,3U,1U};r=umi_ui_ent_selection_range_normalize(r);/* Apply this branch only when its contract condition is satisfied. */ if(r.row_first!=2U||r.column_first!=1U||!umi_ui_ent_selection_range_contains(r,3U,2U))return 1;puts("ok");return 0;}
