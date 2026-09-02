/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_frontend_conformance.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web frontend conformance.
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
#include "umicom/frontend/native_web/frontend_conformance.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebBrowserCapability c; UmiNativeWebConformanceResult r; umi_native_web_browser_capability_init(&c); CHECK(umi_native_web_frontend_conformance(UMI_NATIVE_WEB_CAP_DOM_PATCH|UMI_NATIVE_WEB_CAP_ACCESSIBILITY,&c,&r)==UMI_STATUS_OK); CHECK(r.conformant); CHECK(r.score_percent==100U);
    return 0;
}
