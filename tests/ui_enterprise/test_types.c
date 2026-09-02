/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_types.c
 *
 * PURPOSE:
 *   Exercise the types enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/types.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){char x[8];UmiUiEntSpan s={3U,4U};/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(x,sizeof x,"abc")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_id_valid(x))return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_span_end(s)!=7U||!umi_ui_ent_span_contains(s,6U)||umi_ui_ent_span_contains(s,7U))return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_clamp_size(10U,2U,8U)!=8U)return 4;puts("ok");return 0;}
