/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_protocol_contract.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the protocol contract Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/protocol_contract.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricProtocolContract item;
    CHECK(umi_fabric_protocol_contract_init(&item,"http.v1",UMI_FABRIC_PROTOCOL_HTTP,(UmiFabricVersion){1U,0U,0U},(UmiFabricVersion){1U,9U,0U},false,false)==UMI_STATUS_OK);
    CHECK(item.maximum_version.minor==9U);
    return 0;
}
