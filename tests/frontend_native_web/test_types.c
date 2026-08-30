/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_types.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web types.
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
#include "umicom/frontend/native_web/types.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    char b[16];
    CHECK(umi_native_web_copy_text(b,sizeof(b),"web") == UMI_STATUS_OK);
    CHECK(strcmp(b,"web") == 0);
    CHECK(strcmp(umi_native_web_transport_text(UMI_NATIVE_WEB_TRANSPORT_SSE),"sse") == 0);
    return 0;
}
