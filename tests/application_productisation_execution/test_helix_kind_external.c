/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_helix_kind_external.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: helix kind external.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_helix_kind_external(void)
{
    UmiProductExecutionWorkItem item; UmiProductisationCompletionStep s=test_step(UMI_PRODUCTISATION_STAGE_REMAINING_APPLICATIONS,UMI_PRODUCTISATION_SEVERITY_HIGH,UMI_PRODUCTISATION_OWNER_EXTERNAL_ADAPTER,UMI_PRODUCTISATION_GAP_EXTERNAL_ADAPTER,"x"); assert(umi_product_execution_work_item_init(&item,0U,&s,2U)==UMI_STATUS_OK); assert(umi_product_execution_helix_action_kind(&item)==UMI_HELIX_ACTION_REVIEW);
}
