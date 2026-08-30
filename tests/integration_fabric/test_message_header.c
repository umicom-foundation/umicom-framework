/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_message_header.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the message header Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/message_header.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricMessageHeader item;
    CHECK(umi_fabric_message_header_init(&item,"m1","c1","tenant","application/json",100U)==UMI_STATUS_OK);
    CHECK(strcmp(item.correlation_id,"c1")==0);
    return 0;
}
