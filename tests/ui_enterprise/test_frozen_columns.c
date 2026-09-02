/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_frozen_columns.c
 *
 * PURPOSE:
 *   Exercise the frozen columns enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/frozen_columns.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntFrozenColumns v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_frozen_columns_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_frozen_columns_validate(&v))return 9;puts("ok");return 0;}
