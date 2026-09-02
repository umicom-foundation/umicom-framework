/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_ui_event.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web ui event.
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
#include "umicom/frontend/native_web/ui_event.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebUiEvent e; CHECK(umi_native_web_ui_event_init(&e,UMI_NATIVE_WEB_EVENT_CLICK,"b","","",1U)==UMI_STATUS_OK); CHECK(e.kind==UMI_NATIVE_WEB_EVENT_CLICK);
    return 0;
}
