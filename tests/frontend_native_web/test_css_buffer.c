/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_css_buffer.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web css buffer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/css_buffer.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebCssBuffer b; umi_native_web_css_buffer_init(&b);
    CHECK(umi_native_web_css_buffer_rule(&b,".x","display","grid") == UMI_STATUS_OK);
    CHECK(strstr(b.data,"display:grid") != NULL);
    return 0;
}
