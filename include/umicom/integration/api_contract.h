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
/**
 * Represent the integration api operation data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationApiOperation {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char method[16U];
    char path[UMI_INTEGRATION_DESIGNER_URI_CAPACITY];
    char request_schema_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char response_schema_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    bool streaming;
} UmiIntegrationApiOperation;
/**
 * Represent the integration api contract data shared with callers of this public contract.
 */
typedef struct UmiIntegrationApiContract {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char title[UMI_INTEGRATION_DESIGNER_NAME_CAPACITY];
    char version[32U];
    char profile_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationApiOperation operations[UMI_INTEGRATION_DESIGNER_MAX_OPERATIONS];
    size_t operation_count;
} UmiIntegrationApiContract;
/**
 * Initialise integration api contract from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_integration_api_contract_init(UmiIntegrationApiContract *contract,const char *id,const char *title,const char *version,const char *profile_id);
/**
 * Provide the integration api contract add operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_api_contract_add_operation(UmiIntegrationApiContract *contract,const UmiIntegrationApiOperation *operation);
/**
 * Provide the integration api contract find operation operation used by this module and
 * its client applications.
 */
const UmiIntegrationApiOperation *umi_integration_api_contract_find_operation(const UmiIntegrationApiContract *contract,const char *operation_id);
/**
 * Check that integration api contract satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_integration_api_contract_validate(const UmiIntegrationApiContract *contract,UmiIntegrationDesignerValidation *validation);
#endif
