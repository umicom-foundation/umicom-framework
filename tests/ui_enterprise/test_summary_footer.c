/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_summary_footer.c
 *
 * PURPOSE:
 *   Exercise the summary footer enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/summary_footer.h"
#include <stdio.h>
int main(void){UmiUiEntSummaryFooter f;double v;umi_ui_ent_summary_footer_init(&f);umi_ui_ent_summary_footer_set(&f,"total",12.5);if(!umi_ui_ent_summary_footer_get(&f,"total",&v)||v!=12.5)return 1;puts("ok");return 0;}
