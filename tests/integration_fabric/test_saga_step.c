/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_saga_step.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the saga step Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/saga_step.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricSagaStep item;
    CHECK(umi_fabric_saga_step_init(&item,"reserve","reserve","release",true)==UMI_STATUS_OK);
    CHECK(item.compensation_required);
    return 0;
}
