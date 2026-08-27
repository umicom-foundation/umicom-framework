/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_dependency_duplicate.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: dependency duplicate.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_dependency_duplicate(void)
{
    UmiProductExecutionDependencyGraph g; umi_product_execution_dependency_graph_init(&g); assert(umi_product_execution_dependency_add(&g,0U,1U,2U)==UMI_STATUS_OK); assert(umi_product_execution_dependency_add(&g,0U,1U,2U)==UMI_STATUS_ALREADY_EXISTS);
}
