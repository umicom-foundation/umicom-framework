/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_streaming_window.c
 *
 * PURPOSE:
 *   Exercise the streaming window enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/streaming_window.h"
#include <stdio.h>
int main(void){UmiUiEntStreamingWindow w;umi_ui_ent_streaming_window_init(&w,100U);if(umi_ui_ent_streaming_window_accept(&w,2U,10U,20U)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_streaming_window_accept(&w,1U,0U,1U)!=UMI_STATUS_INVALID_STATE)return 2;puts("ok");return 0;}
