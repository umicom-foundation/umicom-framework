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
#include "value_archive/construction_cases.h"
#include <string.h>
#include "umicom/integration/api_contract.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted api contract value. */
static int UmiIntegrationApiContractConstructionCases(void)
{
    UmiIntegrationApiContract *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_integration_api_contract_init(value, "orders", "Orders", "1.0", "orders-api") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_api_contract_init(value, oversized, "Orders", "1.0", "orders-api") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_api_contract_init(value, value->id, "Orders", "1.0", "orders-api") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "orders") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->title) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_api_contract_init(value, "orders", oversized, "1.0", "orders-api") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_api_contract_init(value, "orders", value->title, "1.0", "orders-api") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->title, "Orders") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->version) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_api_contract_init(value, "orders", "Orders", oversized, "orders-api") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_api_contract_init(value, "orders", "Orders", value->version, "orders-api") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->version, "1.0") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->profile_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_api_contract_init(value, "orders", "Orders", "1.0", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_api_contract_init(value, "orders", "Orders", "1.0", value->profile_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->profile_id, "orders-api") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiIntegrationApiContractConstructionCases() != 0) return 1;

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
