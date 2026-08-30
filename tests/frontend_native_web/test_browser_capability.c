/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_browser_capability.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web browser capability.
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
#include "umicom/frontend/native_web/browser_capability.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebBrowserCapability c; umi_native_web_browser_capability_init(&c); CHECK(umi_native_web_browser_capability_supports(&c,UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_ACCESSIBILITY));
    return 0;
}
