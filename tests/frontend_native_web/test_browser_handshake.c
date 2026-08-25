/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_browser_handshake.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web browser handshake.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/browser_handshake.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebBrowserCapability c; UmiNativeWebBrowserHandshake h; umi_native_web_browser_capability_init(&c); c.flags|=UMI_NATIVE_WEB_CAP_WEBSOCKET; CHECK(umi_native_web_browser_handshake_negotiate(&c,c.flags,&h)==UMI_STATUS_OK); CHECK(h.transport==UMI_NATIVE_WEB_TRANSPORT_WEBSOCKET);
    return 0;
}
