/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/builtin_providers.c
 *
 * PURPOSE:
 *   Enumerate and install built-in build/run providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Return the number of records represented by developer project builtin provider without
 * changing their state.
 */
size_t umi_developer_project_builtin_provider_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

/*
 * Find developer project builtin provider while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectLanguageProvider *
umi_developer_project_builtin_provider_at(size_t index)
{
    return index < umi_developer_project_builtin_provider_count()
        ? FACTORIES[index]()
        : NULL;
}

/*
 * Add developer project builtin providers only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_project_builtin_providers_register(
    UmiDeveloperProjectProviderRegistry *registry)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_developer_project_builtin_provider_count();
         ++index) {
        const UmiStatus status =
            umi_developer_project_provider_registry_register(
                registry, FACTORIES[index]());
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
