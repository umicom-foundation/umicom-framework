/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/api_contract.h
 *
 * PURPOSE:
 *   Publish the public api contract contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_API_CONTRACT_H
#define UMICOM_INTEGRATION_API_CONTRACT_H
#include "umicom/integration/designer_types.h"
typedef struct UmiIntegrationApiOperation {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char method[16U];
    char path[UMI_INTEGRATION_DESIGNER_URI_CAPACITY];
    char request_schema_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char response_schema_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    bool streaming;
} UmiIntegrationApiOperation;
typedef struct UmiIntegrationApiContract {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char title[UMI_INTEGRATION_DESIGNER_NAME_CAPACITY];
    char version[32U];
    char profile_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationApiOperation operations[UMI_INTEGRATION_DESIGNER_MAX_OPERATIONS];
    size_t operation_count;
} UmiIntegrationApiContract;
UmiStatus umi_integration_api_contract_init(UmiIntegrationApiContract *contract,const char *id,const char *title,const char *version,const char *profile_id);
UmiStatus umi_integration_api_contract_add_operation(UmiIntegrationApiContract *contract,const UmiIntegrationApiOperation *operation);
const UmiIntegrationApiOperation *umi_integration_api_contract_find_operation(const UmiIntegrationApiContract *contract,const char *operation_id);
UmiStatus umi_integration_api_contract_validate(const UmiIntegrationApiContract *contract,UmiIntegrationDesignerValidation *validation);
#endif
