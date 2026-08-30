/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/language_provider.c
 *
 * PURPOSE:
 *   Validate project/language provider contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/language_provider.h"

UmiStatus umi_developer_project_language_provider_validate(
    const UmiDeveloperProjectLanguageProvider *provider)
{
    if (provider == NULL ||
        provider->structure_size != sizeof(*provider) ||
        provider->api_version != UMI_DEVELOPER_PROJECT_API_VERSION ||
        provider->provider_id == NULL || provider->provider_id[0] == '\0' ||
        provider->title == NULL || provider->title[0] == '\0' ||
        provider->primary_language_id == NULL ||
        provider->primary_language_id[0] == '\0' ||
        provider->build_system < UMI_DEVELOPER_PROJECT_BUILD_NONE ||
        provider->build_system > UMI_DEVELOPER_PROJECT_BUILD_CUSTOM ||
        (provider->plan_build == NULL && provider->plan_run == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
