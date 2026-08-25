/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_fabric_health.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the fabric health Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/fabric_health.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricHealthSummary s={UMI_FABRIC_STATE_READY,UMI_FABRIC_STATE_DEGRADED,UMI_FABRIC_STATE_READY,UMI_FABRIC_STATE_READY,0,0U}; umi_fabric_health_evaluate(&s); CHECK(s.overall_state==UMI_FABRIC_STATE_DEGRADED); s.workflow_state=UMI_FABRIC_STATE_FAILED; umi_fabric_health_evaluate(&s); CHECK(s.overall_state==UMI_FABRIC_STATE_BLOCKED&&s.blockers==1U);
    return 0;
}
