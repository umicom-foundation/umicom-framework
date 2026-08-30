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
int main(void){UmiUiEntRowCache c;uint64_t r=0;umi_ui_ent_row_cache_init(&c);if(umi_ui_ent_row_cache_put(&c,7U,9U)!=UMI_STATUS_OK)return 1;if(!umi_ui_ent_row_cache_get(&c,7U,&r)||r!=9U)return 2;puts("ok");return 0;}
