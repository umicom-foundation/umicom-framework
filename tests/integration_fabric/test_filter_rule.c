/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_filter_rule.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the filter rule Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/filter_rule.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricFilterExpression e; CHECK(umi_fabric_filter_expression_init(&e,"type","prefix","trade.")==UMI_STATUS_OK); CHECK(umi_fabric_filter_rule_match(&e,"type","trade.new")); CHECK(!umi_fabric_filter_rule_match(&e,"type","risk.new"));
    return 0;
}
