/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_provider_registry.c
 *
 * PURPOSE:
 *   Verify deterministic provider selection by build system/language/priority.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_project/builtin_providers.h"

int main(void)
{
    UmiDeveloperProjectProviderRegistry *registry = NULL;
    UmiDeveloperProjectModel model;
    const UmiDeveloperProjectLanguageProvider *provider;

    assert(umi_developer_project_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_project_builtin_providers_register(registry) ==
           UMI_STATUS_OK);

    umi_developer_project_model_init(&model, "alpha", "Alpha");
    (void)strcpy(model.root, "C:/work/alpha");
    (void)strcpy(model.primary_language_id, "developer.language.c23");
    model.build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE;

    provider = umi_developer_project_provider_registry_select(
        registry, &model);
    assert(provider != NULL);
    assert(strcmp(provider->provider_id, "developer.provider.cmake") == 0);

    umi_developer_project_provider_registry_destroy(registry);
    return 0;
}
