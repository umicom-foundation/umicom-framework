/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_designer_types.c
 *
 * PURPOSE:
 *   Implement the test integration designer types behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Designer types | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/integration/designer_types.h"
int main(void)
{
    UmiIntegrationDesignerRecord record = {0};
    UmiIntegrationDesignerValidation validation = {0};
    assert(umi_integration_designer_record_set(&record,"desk","treasury") == UMI_STATUS_OK);
    assert(umi_integration_designer_record_set(&record,"desk","markets") == UMI_STATUS_OK);
    assert(record.count == 1U && strcmp(umi_integration_designer_record_get(&record,"desk"),"markets") == 0);
    assert(umi_integration_designer_validation_add(&validation,"payload","Missing field") == UMI_STATUS_OK);
    assert(validation.count == 1U && strcmp(umi_integration_designer_transport_text(UMI_INTEGRATION_DESIGNER_GRAPHQL),"GraphQL") == 0);
    return 0;
}
