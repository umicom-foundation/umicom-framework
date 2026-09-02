/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_split_rule.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the split rule Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/split_rule.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFabricSplitRule item;
    CHECK(umi_fabric_split_rule_init(&item,"s","items[*]",100U,true)==UMI_STATUS_OK);
    CHECK(item.maximum_parts==100U);
    return 0;
}
