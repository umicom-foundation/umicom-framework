/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_trace_context.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the trace context Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/trace_context.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricTraceContext p,c; CHECK(umi_fabric_trace_context_root(&p,"t","s1","c",true)==UMI_STATUS_OK); CHECK(umi_fabric_trace_context_child(&p,"s2",&c)==UMI_STATUS_OK); CHECK(strcmp(c.parent_span_id,"s1")==0&&strcmp(c.trace_id,"t")==0);
    return 0;
}
