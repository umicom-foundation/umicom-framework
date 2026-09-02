/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_types.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the types Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/types.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    char b[16]; UmiFabricVersion a={1U,2U,0U}, c={1U,3U,0U};
    CHECK(umi_fabric_copy_text(b,sizeof(b),"fabric")==UMI_STATUS_OK);
    CHECK(strcmp(b,"fabric")==0);
    CHECK(umi_fabric_hash64("abc",3U,0U)!=0U);
    CHECK(umi_fabric_version_compare(a,c)<0);
    CHECK(strcmp(umi_fabric_state_text(UMI_FABRIC_STATE_READY),"ready")==0);
    return 0;
}
