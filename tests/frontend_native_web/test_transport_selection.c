/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_transport_selection.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web transport selection.
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
#include "umicom/frontend/native_web/transport_selection.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebBrowserCapability c; umi_native_web_browser_capability_init(&c); c.flags|=UMI_NATIVE_WEB_CAP_WEBSOCKET; CHECK(umi_native_web_transport_select(&c,~0ULL)==UMI_NATIVE_WEB_TRANSPORT_WEBSOCKET);
    return 0;
}
