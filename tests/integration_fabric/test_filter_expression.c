/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_filter_expression.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the filter expression Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/filter_expression.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricFilterExpression item;
    CHECK(umi_fabric_filter_expression_init(&item,"type","prefix","trade.")==UMI_STATUS_OK);
    CHECK(strcmp(item.operation,"prefix")==0);
    return 0;
}
