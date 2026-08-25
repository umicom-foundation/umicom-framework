/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_compensation_record.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the compensation record Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/compensation_record.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricCompensationRecord r; CHECK(umi_fabric_compensation_record_init(&r,"wf","s")==UMI_STATUS_OK); umi_fabric_compensation_record_attempt(&r,false,1U); umi_fabric_compensation_record_attempt(&r,true,2U); CHECK(r.attempts==2U&&r.succeeded);
    return 0;
}
