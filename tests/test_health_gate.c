/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_health_gate.c
 *
 * PURPOSE:
 *   Verify health gates deny missing or unhealthy components and allow ready
 *   components.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include "umicom/resilience/health_gate.h"
int main(void) {
    UmiHealthRegistry *r = NULL; UmiHealthGate *g = NULL; char reason[256];
    assert(umi_health_registry_create(&r) == UMI_STATUS_OK); assert(umi_health_gate_create(&g) == UMI_STATUS_OK);
    assert(umi_health_gate_require(g,"data",UMI_HEALTH_READY) == UMI_STATUS_OK);
    assert(umi_health_gate_evaluate(g,r,reason,sizeof(reason)) == UMI_STATUS_UNAVAILABLE);
    assert(umi_health_registry_update(r,"data",UMI_HEALTH_READY,"ready",1U) == UMI_STATUS_OK);
    assert(umi_health_gate_evaluate(g,r,reason,sizeof(reason)) == UMI_STATUS_OK);
    umi_health_gate_destroy(g); umi_health_registry_destroy(r); return 0;
}
