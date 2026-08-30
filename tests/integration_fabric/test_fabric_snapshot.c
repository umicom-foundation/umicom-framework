/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_fabric_snapshot.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the fabric snapshot Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/fabric_snapshot.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricSnapshot s; CHECK(umi_fabric_snapshot_build(1U,2U,3U,4U,5U,6U,UMI_FABRIC_STATE_READY,7U,&s)==UMI_STATUS_OK); CHECK(s.fingerprint!=0U&&s.routes==6U);
    return 0;
}
