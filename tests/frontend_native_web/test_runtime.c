/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_runtime.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web runtime.
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
#include "umicom/frontend/native_web/runtime.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebRuntime rt; UmiNativeWebRenderContext c; UmiNativeWebSurface s={0}; UmiNativeWebRenderResult r; CHECK(umi_native_web_runtime_init(&rt)==UMI_STATUS_OK); CHECK(rt.renderers.count>=20U); CHECK(umi_native_web_render_context_init(&c,"s","/")==UMI_STATUS_OK); strcpy(s.surface_id,"wb"); strcpy(s.semantic_contract,"Umicom.Ui.Workbench"); strcpy(s.title,"Studio"); CHECK(umi_native_web_runtime_render(&rt,&c,&s,&r)==UMI_STATUS_OK); CHECK(strstr(r.markup.data,"umicom-workbench")!=NULL);
    return 0;
}
