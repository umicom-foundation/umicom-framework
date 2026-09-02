/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_summary_footer.c
 *
 * PURPOSE:
 *   Exercise the summary footer enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/summary_footer.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntSummaryFooter f;double v;umi_ui_ent_summary_footer_init(&f);umi_ui_ent_summary_footer_set(&f,"total",12.5);/* Apply this branch only when its contract condition is satisfied. */ if(!umi_ui_ent_summary_footer_get(&f,"total",&v)||v!=12.5)return 1;puts("ok");return 0;}
