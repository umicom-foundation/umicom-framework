/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_render_node_bridge.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web render node bridge.
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
#include "umicom/frontend/native_web/render_node_bridge.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiFrontendRenderNodeSnapshot s={0}; UmiNativeWebSemanticElement e; strcpy(s.id,"n1"); strcpy(s.markup,"Hello"); s.order=4; s.revision=7; CHECK(umi_native_web_render_node_bridge(&s,&e)==UMI_STATUS_OK); CHECK(strcmp(e.text,"Hello")==0); CHECK(e.order==4);
    return 0;
}
