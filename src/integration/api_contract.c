/* Umicom Framework | API contracts | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/api_contract.h"
#include <string.h>

UmiStatus umi_integration_api_contract_init(UmiIntegrationApiContract *contract,const char *id,const char *title,const char *version,const char *profile_id)
{
    UmiStatus status;
    if (contract == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(contract,0,sizeof(*contract));
    status = umi_integration_designer_copy(contract->id,sizeof(contract->id),id);
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(contract->title,sizeof(contract->title),title);
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(contract->version,sizeof(contract->version),version);
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(contract->profile_id,sizeof(contract->profile_id),profile_id);
    return status;
}

const UmiIntegrationApiOperation *umi_integration_api_contract_find_operation(const UmiIntegrationApiContract *contract,const char *operation_id)
{
    size_t index;
    if (contract == NULL || operation_id == NULL) return NULL;
    for (index = 0U; index < contract->operation_count; ++index) if (strcmp(contract->operations[index].id,operation_id) == 0) return &contract->operations[index];
    return NULL;
}

UmiStatus umi_integration_api_contract_add_operation(UmiIntegrationApiContract *contract,const UmiIntegrationApiOperation *operation)
{
    if (contract == NULL || operation == NULL || operation->id[0] == '\0' || operation->path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_integration_api_contract_find_operation(contract,operation->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (contract->operation_count >= UMI_INTEGRATION_DESIGNER_MAX_OPERATIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    contract->operations[contract->operation_count] = *operation;
    contract->operation_count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_integration_api_contract_validate(const UmiIntegrationApiContract *contract,UmiIntegrationDesignerValidation *validation)
{
    if (contract == NULL || validation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    validation->count = 0U;
    if (contract->id[0] == '\0') (void)umi_integration_designer_validation_add(validation,"contract.id","Contract id is required");
    if (contract->profile_id[0] == '\0') (void)umi_integration_designer_validation_add(validation,"contract.profile_id","Connection profile is required");
    if (contract->operation_count == 0U) (void)umi_integration_designer_validation_add(validation,"contract.operations","At least one operation is required");
    return validation->count == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
