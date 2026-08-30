/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_node_graph_renderer.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web node graph renderer.
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
#include "umicom/frontend/native_web/node_graph_renderer.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebRenderContext c; UmiNativeWebSurface s={0}; UmiNativeWebRenderResult r; UmiNativeWebRendererDescriptor d; CHECK(umi_native_web_render_context_init(&c,"s","/")==UMI_STATUS_OK); strcpy(s.surface_id,"x"); strcpy(s.semantic_contract,"Umicom.Ui.NodeGraph"); strcpy(s.title,"Title"); umi_native_web_render_result_init(&r,1U); CHECK(umi_native_web_node_graph_render(&c,&s,&r)==UMI_STATUS_OK); CHECK(strstr(r.markup.data,"umicom-node-graph")!=NULL); d=umi_native_web_node_graph_renderer_descriptor(); CHECK(umi_native_web_renderer_descriptor_validate(&d)==UMI_STATUS_OK);
    return 0;
}
