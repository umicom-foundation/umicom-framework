/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_row_cache.c
 *
 * PURPOSE:
 *   Exercise the row cache enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_cache.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntRowCache c;uint64_t r=0;umi_ui_ent_row_cache_init(&c);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_row_cache_put(&c,7U,9U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_row_cache_get(&c,7U,&r)||r!=9U)return 2;puts("ok");return 0;}
