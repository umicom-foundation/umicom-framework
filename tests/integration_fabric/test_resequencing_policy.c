/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_resequencing_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the resequencing policy Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/resequencing_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricResequencingPolicy item;
    CHECK(umi_fabric_resequencing_policy_init(&item,"seq",128U,500U,true)==UMI_STATUS_OK);
    CHECK(item.release_on_timeout);
    return 0;
}
