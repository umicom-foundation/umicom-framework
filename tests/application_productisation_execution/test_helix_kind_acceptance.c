/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_helix_kind_acceptance.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: helix kind acceptance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

/*
 * Exercise test helix kind acceptance and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_helix_kind_acceptance(void)
{
    UmiProductExecutionWorkItem item; UmiProductisationCompletionStep s=test_step(UMI_PRODUCTISATION_STAGE_ACCEPTANCE,UMI_PRODUCTISATION_SEVERITY_HIGH,UMI_PRODUCTISATION_OWNER_FRAMEWORK,UMI_PRODUCTISATION_GAP_ACCEPTANCE_EVIDENCE,"x"); assert(umi_product_execution_work_item_init(&item,0U,&s,2U)==UMI_STATUS_OK); assert(umi_product_execution_helix_action_kind(&item)==UMI_HELIX_ACTION_TEST);
}
