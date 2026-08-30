/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_renderer_registry.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web renderer registry.
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
#include "umicom/frontend/native_web/renderer_registry.h"
static UmiStatus dummy_render(const UmiNativeWebRenderContext *c,const UmiNativeWebSurface *s,UmiNativeWebRenderResult *r){(void)c;(void)s;(void)r;return UMI_STATUS_OK;}
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebRendererRegistry r; UmiNativeWebRendererDescriptor d={0}; umi_native_web_renderer_registry_init(&r); strcpy(d.renderer_id,"r1"); strcpy(d.semantic_contract,"X"); d.render=dummy_render; CHECK(umi_native_web_renderer_registry_add(&r,&d)==UMI_STATUS_OK); CHECK(umi_native_web_renderer_registry_resolve(&r,"X")!=NULL);
    return 0;
}
