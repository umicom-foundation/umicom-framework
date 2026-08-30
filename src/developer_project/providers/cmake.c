/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/providers/cmake.c
 *
 * PURPOSE:
 *   Implement CMake/Ninja build/run planning through safe Framework argv
 *   plans. No shell interpolation or Studio-specific process logic is used.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/providers/cmake.h"

#include "umicom/developer_project/model.h"
#include "umicom/developer_project/build_plan.h"
#include "umicom/developer_project/run_plan.h"


static UmiStatus plan_build(
    const UmiDeveloperProjectModel *model,
    int include_test,
    int include_package,
    UmiDeveloperProjectBuildPlan *out_plan)
{
    return umi_developer_project_build_plan_cmake(
        out_plan,
        model->root,
        model->build_directory,
        model->preset,
        "Debug",
        include_test,
        include_package);
}

static UmiStatus plan_run(
    const UmiDeveloperProjectModel *model,
    UmiDeveloperProjectRunPlan *out_plan)
{
    if (model->executable[0] == '\0') return UMI_STATUS_NOT_FOUND;

    return umi_developer_project_run_plan_native(
        out_plan,
        model->primary_language_id,
        model->executable,
        model->root);
}


const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_cmake(void)
{
    static const UmiDeveloperProjectLanguageProvider provider = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperProjectLanguageProvider),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .provider_id = "developer.provider.cmake",
        .title = "CMake/Ninja",
        .primary_language_id = "*",
        .build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE,
        .priority = 100,
        .built_in = 1,
        .plan_build = plan_build,
        .plan_run = plan_run
    };

    return &provider;
}
