/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_dependency_add.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: dependency add.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

/* Add test dependency only after its inputs and available capacity have been checked. */
void test_dependency_add(void)
{
    UmiProductExecutionDependencyGraph g; umi_product_execution_dependency_graph_init(&g); assert(umi_product_execution_dependency_add(&g,0U,1U,2U)==UMI_STATUS_OK); assert(g.count==1U);
}
