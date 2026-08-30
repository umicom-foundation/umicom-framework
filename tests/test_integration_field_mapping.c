/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_field_mapping.c
 *
 * PURPOSE:
 *   Implement the test integration field mapping behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Field mapping | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/integration/field_mapping.h"
int main(void)
{
    UmiIntegrationFieldMapping mapping;
    UmiIntegrationFieldRule copy = {0};
    UmiIntegrationFieldRule constant = {0};
    UmiIntegrationDesignerRecord source = {0};
    UmiIntegrationDesignerRecord target = {0};
    UmiIntegrationDesignerValidation validation = {0};
    assert(umi_integration_field_mapping_init(&mapping,"order-map","source","target") == UMI_STATUS_OK);
    (void)umi_integration_designer_copy(copy.source,sizeof(copy.source),"desk"); (void)umi_integration_designer_copy(copy.target,sizeof(copy.target),"BOOK"); copy.transform = UMI_INTEGRATION_DESIGNER_UPPERCASE; copy.required = true;
    (void)umi_integration_designer_copy(constant.target,sizeof(constant.target),"source"); (void)umi_integration_designer_copy(constant.constant_value,sizeof(constant.constant_value),"umicom"); constant.transform = UMI_INTEGRATION_DESIGNER_CONSTANT;
    assert(umi_integration_field_mapping_add(&mapping,&copy) == UMI_STATUS_OK);
    assert(umi_integration_field_mapping_add(&mapping,&constant) == UMI_STATUS_OK);
    assert(umi_integration_designer_record_set(&source,"desk","treasury") == UMI_STATUS_OK);
    assert(umi_integration_field_mapping_apply(&mapping,&source,&target,&validation) == UMI_STATUS_OK);
    assert(strcmp(umi_integration_designer_record_get(&target,"BOOK"),"TREASURY") == 0);
    return 0;
}
