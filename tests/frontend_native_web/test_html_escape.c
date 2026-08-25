/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_html_escape.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web html escape.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/html_escape.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebMarkupBuffer b; umi_native_web_markup_buffer_init(&b);
    CHECK(umi_native_web_html_escape_attribute(&b,"A&B\"") == UMI_STATUS_OK);
    CHECK(strstr(b.data,"&amp;") != NULL); CHECK(strstr(b.data,"&quot;") != NULL);
    return 0;
}
