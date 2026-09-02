/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/connection_profile.h
 *
 * PURPOSE:
 *   Publish the public connection profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_CONNECTION_PROFILE_H
#define UMICOM_INTEGRATION_CONNECTION_PROFILE_H
#include "umicom/integration/designer_types.h"
/**
 * Represent the integration connection profile data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationConnectionProfile {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char name[UMI_INTEGRATION_DESIGNER_NAME_CAPACITY];
    UmiIntegrationDesignerTransport transport;
    char base_uri[UMI_INTEGRATION_DESIGNER_URI_CAPACITY];
    UmiIntegrationDesignerAuth auth;
    char secret_reference[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    unsigned timeout_ms;
    bool enabled;
} UmiIntegrationConnectionProfile;
/**
 * Represent the integration connection catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationConnectionCatalogue {
    UmiIntegrationConnectionProfile items[UMI_INTEGRATION_DESIGNER_MAX_PROFILES];
    size_t count;
} UmiIntegrationConnectionCatalogue;
/**
 * Check that integration connection profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_integration_connection_profile_validate(const UmiIntegrationConnectionProfile *profile,UmiIntegrationDesignerValidation *validation);
/**
 * Add integration connection catalogue only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_integration_connection_catalogue_add(UmiIntegrationConnectionCatalogue *catalogue,const UmiIntegrationConnectionProfile *profile);
/**
 * Find integration connection catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiIntegrationConnectionProfile *umi_integration_connection_catalogue_find(const UmiIntegrationConnectionCatalogue *catalogue,const char *id);
/**
 * Remove integration connection catalogue while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_integration_connection_catalogue_remove(UmiIntegrationConnectionCatalogue *catalogue,const char *id);
#endif
