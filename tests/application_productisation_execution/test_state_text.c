/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_state_text.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: state text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_state_text(void)
{
    assert(strcmp(umi_product_execution_state_text(UMI_PRODUCT_EXECUTION_READY), "ready") == 0);
}
