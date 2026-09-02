/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_pinned_rows.c
 *
 * PURPOSE:
 *   Exercise the pinned rows enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/pinned_rows.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntPinnedRows p;umi_ui_ent_pinned_rows_init(&p);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_pinned_rows_add_top(&p,7U)!=UMI_STATUS_OK||!umi_ui_ent_pinned_rows_contains(&p,7U))return 1;puts("ok");return 0;}
