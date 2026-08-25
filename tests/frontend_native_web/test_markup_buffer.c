/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_markup_buffer.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web markup buffer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/markup_buffer.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebMarkupBuffer b; umi_native_web_markup_buffer_init(&b);
    CHECK(umi_native_web_markup_buffer_append(&b,"<main>") == UMI_STATUS_OK);
    CHECK(umi_native_web_markup_buffer_append(&b,"ok") == UMI_STATUS_OK);
    CHECK(strcmp(b.data,"<main>ok") == 0);
    return 0;
}
