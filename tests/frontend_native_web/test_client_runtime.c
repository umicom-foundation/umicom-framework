/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_client_runtime.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web client runtime.
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
#include "umicom/frontend/native_web/client_runtime.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebMarkupBuffer js; CHECK(umi_native_web_client_runtime_emit("/_umi/event",&js)==UMI_STATUS_OK); CHECK(strstr(js.data,"window.UmicomWeb")!=NULL); CHECK(!umi_native_web_client_runtime_uses_webassembly(&js));
    return 0;
}
