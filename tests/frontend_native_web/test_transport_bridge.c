/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_transport_bridge.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web transport bridge.
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
#include "umicom/frontend/native_web/transport_bridge.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiFrontendTransportSnapshot s={0}; UmiNativeWebTransportState o; strcpy(s.id,"t1"); strcpy(s.kind,"sse"); strcpy(s.endpoint,"/_umi/events"); CHECK(umi_native_web_transport_bridge(&s,&o)==UMI_STATUS_OK); CHECK(o.kind==UMI_NATIVE_WEB_TRANSPORT_SSE);
    return 0;
}
