/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_dead_letter_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the dead letter policy Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/dead_letter_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricDeadLetterPolicy item;
    CHECK(umi_fabric_dead_letter_policy_init(&item,"dlq","topic://dead",3U,60000U,false)==UMI_STATUS_OK);
    CHECK(item.after_attempts==3U);
    return 0;
}
