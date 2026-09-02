/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_streaming_window.c
 *
 * PURPOSE:
 *   Exercise the streaming window enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/streaming_window.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntStreamingWindow w;umi_ui_ent_streaming_window_init(&w,100U);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_streaming_window_accept(&w,2U,10U,20U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_streaming_window_accept(&w,1U,0U,1U)!=UMI_STATUS_INVALID_STATE)return 2;puts("ok");return 0;}
