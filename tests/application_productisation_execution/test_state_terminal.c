/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_state_terminal.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: state terminal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_state_terminal(void)
{
    assert(umi_product_execution_state_terminal(UMI_PRODUCT_EXECUTION_SUCCEEDED)); assert(!umi_product_execution_state_terminal(UMI_PRODUCT_EXECUTION_RUNNING));
}
