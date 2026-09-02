/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_renderer_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web renderer descriptor.
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
#include "umicom/frontend/native_web/renderer_descriptor.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebSurface s={0}; UmiNativeWebRenderResult r; strcpy(s.surface_id,"p"); strcpy(s.title,"Panel"); umi_native_web_render_result_init(&r,1U); CHECK(umi_native_web_render_surface_wrapper(&s,"umicom-panel","region",&r)==UMI_STATUS_OK); CHECK(strstr(r.markup.data,"umicom-panel")!=NULL);
    return 0;
}
