/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_render_node_bridge.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web render node bridge.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/render_node_bridge.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiFrontendRenderNodeSnapshot s={0}; UmiNativeWebSemanticElement e; strcpy(s.id,"n1"); strcpy(s.markup,"Hello"); s.order=4; s.revision=7; CHECK(umi_native_web_render_node_bridge(&s,&e)==UMI_STATUS_OK); CHECK(strcmp(e.text,"Hello")==0); CHECK(e.order==4);
    return 0;
}
