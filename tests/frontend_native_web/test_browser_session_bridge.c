/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_browser_session_bridge.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web browser session bridge.
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
#include "umicom/frontend/native_web/browser_session_bridge.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiFrontendSessionSnapshot s={0}; UmiNativeWebBrowserSessionState o; strcpy(s.id,"s1"); strcpy(s.route,"/studio"); s.connected=1; s.revision=2; CHECK(umi_native_web_browser_session_bridge(&s,&o)==UMI_STATUS_OK); CHECK(o.connected); CHECK(strcmp(o.route,"/studio")==0);
    return 0;
}
