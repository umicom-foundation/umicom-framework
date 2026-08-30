/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_message_envelope.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the message envelope Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/message_envelope.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricMessageHeader h; UmiFabricMessageEnvelope e; const char p[]="abc";
    CHECK(umi_fabric_message_header_init(&h,"m","c","t","text/plain",1U)==UMI_STATUS_OK);
    CHECK(umi_fabric_message_envelope_init(&e,&h,p,3U,false)==UMI_STATUS_OK); CHECK(umi_fabric_message_envelope_matches(&e,p,3U));
    return 0;
}
