/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/connection_profile.c
 *
 * PURPOSE:
 *   Implement the connection profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Integration connection profiles | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/connection_profile.h"
#include <string.h>

UmiStatus umi_integration_connection_profile_validate(const UmiIntegrationConnectionProfile *profile,UmiIntegrationDesignerValidation *validation)
{
    if (profile == NULL || validation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    validation->count = 0U;
    if (profile->id[0] == '\0') (void)umi_integration_designer_validation_add(validation,"profile.id","Connection id is required");
    if (profile->name[0] == '\0') (void)umi_integration_designer_validation_add(validation,"profile.name","Connection name is required");
    if (profile->transport < UMI_INTEGRATION_DESIGNER_REST || profile->transport > UMI_INTEGRATION_DESIGNER_MESSAGE_BUS) (void)umi_integration_designer_validation_add(validation,"profile.transport","A supported transport is required");
    if (profile->base_uri[0] == '\0') (void)umi_integration_designer_validation_add(validation,"profile.base_uri","Endpoint or topic address is required");
    if (profile->timeout_ms == 0U) (void)umi_integration_designer_validation_add(validation,"profile.timeout_ms","Timeout must be greater than zero");
    if (profile->auth != UMI_INTEGRATION_DESIGNER_AUTH_NONE && profile->secret_reference[0] == '\0') (void)umi_integration_designer_validation_add(validation,"profile.secret_reference","Authenticated profiles require a secret reference");
    return validation->count == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

const UmiIntegrationConnectionProfile *umi_integration_connection_catalogue_find(const UmiIntegrationConnectionCatalogue *catalogue,const char *id)
{
    size_t index;
    if (catalogue == NULL || id == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) if (strcmp(catalogue->items[index].id,id) == 0) return &catalogue->items[index];
    return NULL;
}

UmiStatus umi_integration_connection_catalogue_add(UmiIntegrationConnectionCatalogue *catalogue,const UmiIntegrationConnectionProfile *profile)
{
    UmiIntegrationDesignerValidation validation = {0};
    UmiStatus status;
    if (catalogue == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_integration_connection_profile_validate(profile,&validation);
    if (status != UMI_STATUS_OK) return status;
    if (umi_integration_connection_catalogue_find(catalogue,profile->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count >= UMI_INTEGRATION_DESIGNER_MAX_PROFILES) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count] = *profile;
    catalogue->count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_integration_connection_catalogue_remove(UmiIntegrationConnectionCatalogue *catalogue,const char *id)
{
    size_t index;
    if (catalogue == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].id,id) == 0) {
            if (index + 1U < catalogue->count) (void)memmove(&catalogue->items[index],&catalogue->items[index + 1U],(catalogue->count - index - 1U) * sizeof(catalogue->items[0]));
            catalogue->count -= 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
