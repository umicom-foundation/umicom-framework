/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_event_dispatch.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web event dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/event_dispatch.h"
static UmiStatus handler(void *c,const UmiNativeWebUiEvent *e){int *n=c; (void)e; ++*n; return UMI_STATUS_OK;}
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebEventDispatcher d; UmiNativeWebUiEvent e; int n=0; umi_native_web_event_dispatch_init(&d); CHECK(umi_native_web_event_dispatch_register(&d,UMI_NATIVE_WEB_EVENT_CLICK,handler,&n)==UMI_STATUS_OK); CHECK(umi_native_web_ui_event_init(&e,UMI_NATIVE_WEB_EVENT_CLICK,"x",NULL,NULL,1U)==UMI_STATUS_OK); CHECK(umi_native_web_event_dispatch_one(&d,&e)==UMI_STATUS_OK); CHECK(n==1);
    return 0;
}
