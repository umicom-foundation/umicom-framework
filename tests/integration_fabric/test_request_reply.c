/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_request_reply.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the request reply Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/request_reply.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricMessageHeader q,r; UmiFabricRequestReply s; CHECK(umi_fabric_message_header_init(&q,"q","c","t","x",1U)==UMI_STATUS_OK); CHECK(umi_fabric_message_header_init(&r,"r","c","t","x",2U)==UMI_STATUS_OK); CHECK(umi_fabric_request_reply_begin(&s,&q)==UMI_STATUS_OK); CHECK(umi_fabric_request_reply_complete(&s,&r,200,3U)==UMI_STATUS_OK); CHECK(s.completed);
    return 0;
}
