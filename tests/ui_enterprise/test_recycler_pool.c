/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_recycler_pool.c
 *
 * PURPOSE:
 *   Exercise the recycler pool enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/recycler_pool.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntRecyclerPool p;size_t slot;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_recycler_pool_init(&p,1U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_recycler_pool_acquire(&p,&slot)!=UMI_STATUS_OK||slot!=0U)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_recycler_pool_acquire(&p,&slot)!=UMI_STATUS_CAPACITY_EXCEEDED)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_recycler_pool_release(&p)!=UMI_STATUS_OK)return 4;puts("ok");return 0;}
