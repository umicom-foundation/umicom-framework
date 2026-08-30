/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_api_contract.c
 *
 * PURPOSE:
 *   Implement the test integration api contract behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | API contract | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/integration/api_contract.h"
int main(void)
{
    UmiIntegrationApiContract contract;
    UmiIntegrationApiOperation operation = {0};
    UmiIntegrationDesignerValidation validation = {0};
    assert(umi_integration_api_contract_init(&contract,"orders","Orders","1.0","orders-api") == UMI_STATUS_OK);
    (void)umi_integration_designer_copy(operation.id,sizeof(operation.id),"create-order");
    (void)umi_integration_designer_copy(operation.method,sizeof(operation.method),"POST");
    (void)umi_integration_designer_copy(operation.path,sizeof(operation.path),"/orders");
    assert(umi_integration_api_contract_add_operation(&contract,&operation) == UMI_STATUS_OK);
    assert(umi_integration_api_contract_validate(&contract,&validation) == UMI_STATUS_OK);
    assert(strcmp(umi_integration_api_contract_find_operation(&contract,"create-order")->method,"POST") == 0);
    return 0;
}
