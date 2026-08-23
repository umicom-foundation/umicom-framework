/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/builtin_providers.c
 *
 * PURPOSE:
 *   Enumerate and install built-in build/run providers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/builtin_providers.h"

#include "umicom/developer_project/providers/providers.h"

typedef const UmiDeveloperProjectLanguageProvider *(*ProviderFactory)(void);

static const ProviderFactory FACTORIES[] = {
    umi_developer_project_provider_cmake,
    umi_developer_project_provider_zig,
    umi_developer_project_provider_cargo,
    umi_developer_project_provider_cpython,
    umi_developer_project_provider_bash
};

size_t umi_developer_project_builtin_provider_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiDeveloperProjectLanguageProvider *
umi_developer_project_builtin_provider_at(size_t index)
{
    return index < umi_developer_project_builtin_provider_count()
        ? FACTORIES[index]()
        : NULL;
}

UmiStatus umi_developer_project_builtin_providers_register(
    UmiDeveloperProjectProviderRegistry *registry)
{
    size_t index;

    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    for (index = 0U;
         index < umi_developer_project_builtin_provider_count();
         ++index) {
        const UmiStatus status =
            umi_developer_project_provider_registry_register(
                registry, FACTORIES[index]());
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
