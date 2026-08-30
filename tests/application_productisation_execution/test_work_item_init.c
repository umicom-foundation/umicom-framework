/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_work_item_init.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: work item init.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_work_item_init(void)
{
    UmiProductExecutionWorkItem item; UmiProductisationCompletionStep s=test_step(UMI_PRODUCTISATION_STAGE_FRAMEWORK,UMI_PRODUCTISATION_SEVERITY_HIGH,UMI_PRODUCTISATION_OWNER_FRAMEWORK,UMI_PRODUCTISATION_GAP_MISSING_COMPONENT,"x"); assert(umi_product_execution_work_item_init(&item,3U,&s,2U)==UMI_STATUS_OK); assert(item.plan_index==3U);
}
