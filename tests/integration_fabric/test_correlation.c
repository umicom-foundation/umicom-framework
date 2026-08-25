/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_correlation.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the correlation Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/correlation.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricMessageHeader a,b; CHECK(umi_fabric_message_header_init(&a,"m1","c","t","x",1U)==UMI_STATUS_OK); CHECK(umi_fabric_message_header_init(&b,"m2","c","t","x",2U)==UMI_STATUS_OK); CHECK(umi_fabric_correlation_matches(&a,&b)); CHECK(umi_fabric_correlation_set_causation(&b,"m1")==UMI_STATUS_OK); CHECK(strcmp(b.causation_id,"m1")==0);
    return 0;
}
