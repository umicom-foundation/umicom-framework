/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_request_retry.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the request retry Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/request_retry.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricRequestRetry p; uint64_t d=0U; umi_fabric_request_retry_default(&p); CHECK(umi_fabric_request_retry_delay(&p,3U,&d)==UMI_STATUS_OK); CHECK(d==400U);
    return 0;
}
