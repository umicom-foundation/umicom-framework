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
typedef struct UmiIntegrationConnectionCatalogue {
    UmiIntegrationConnectionProfile items[UMI_INTEGRATION_DESIGNER_MAX_PROFILES];
    size_t count;
} UmiIntegrationConnectionCatalogue;
UmiStatus umi_integration_connection_profile_validate(const UmiIntegrationConnectionProfile *profile,UmiIntegrationDesignerValidation *validation);
UmiStatus umi_integration_connection_catalogue_add(UmiIntegrationConnectionCatalogue *catalogue,const UmiIntegrationConnectionProfile *profile);
const UmiIntegrationConnectionProfile *umi_integration_connection_catalogue_find(const UmiIntegrationConnectionCatalogue *catalogue,const char *id);
UmiStatus umi_integration_connection_catalogue_remove(UmiIntegrationConnectionCatalogue *catalogue,const char *id);
#endif
