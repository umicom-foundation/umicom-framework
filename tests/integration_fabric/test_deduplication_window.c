/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_deduplication_window.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the deduplication window Integration Fabric capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/deduplication_window.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricDeduplicationWindow w; bool dup=false; umi_fabric_deduplication_window_init(&w,100U); CHECK(umi_fabric_deduplication_window_observe(&w,"m1",10U,&dup)==UMI_STATUS_OK); CHECK(!dup); CHECK(umi_fabric_deduplication_window_observe(&w,"m1",20U,&dup)==UMI_STATUS_OK); CHECK(dup); CHECK(umi_fabric_deduplication_window_observe(&w,"m1",200U,&dup)==UMI_STATUS_OK); CHECK(!dup);
    return 0;
}
