/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_transport_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the transport profile Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/transport_profile.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricTransportProfile item;
    CHECK(umi_fabric_transport_profile_init(&item,"secure",1048576U,1000U,5000U,true,true)==UMI_STATUS_OK);
    CHECK(item.tls_required);
    return 0;
}
