/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_render_context.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web render context.
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
#include "umicom/frontend/native_web/render_context.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebRenderContext c; CHECK(umi_native_web_render_context_init(&c,"s1","/")==UMI_STATUS_OK); CHECK(umi_native_web_render_context_validate(&c)==UMI_STATUS_OK); CHECK(strcmp(c.locale,"en-GB")==0);
    return 0;
}
