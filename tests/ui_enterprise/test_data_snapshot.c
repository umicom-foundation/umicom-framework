/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_data_snapshot.c
 *
 * PURPOSE:
 *   Exercise the data snapshot enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/data_snapshot.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntDataSnapshot v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_data_snapshot_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_data_snapshot_validate(&v))return 9;puts("ok");return 0;}
