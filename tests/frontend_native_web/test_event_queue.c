/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_event_queue.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web event queue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/event_queue.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebEventQueue q; UmiNativeWebUiEvent a,b; umi_native_web_event_queue_init(&q); CHECK(umi_native_web_ui_event_init(&a,UMI_NATIVE_WEB_EVENT_CLICK,"x",NULL,NULL,1U)==UMI_STATUS_OK); CHECK(umi_native_web_event_queue_push(&q,&a)==UMI_STATUS_OK); CHECK(umi_native_web_event_queue_pop(&q,&b)==UMI_STATUS_OK); CHECK(b.sequence==1U);
    return 0;
}
