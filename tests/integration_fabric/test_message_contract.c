/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_message_contract.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the message contract Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/message_contract.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricMessageContract item;
    CHECK(umi_fabric_message_contract_init(&item,"trade.v1","trade",(UmiFabricVersion){1U,0U,0U},65536U,UMI_FABRIC_DELIVERY_AT_LEAST_ONCE)==UMI_STATUS_OK);
    CHECK(item.max_payload_bytes==65536U);
    return 0;
}
