/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/providers/cargo.c
 *
 * PURPOSE:
 *   Implement Cargo build/run planning through safe Framework argv
 *   plans. No shell interpolation or Studio-specific process logic is used.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/providers/cargo.h"

#include "umicom/developer_project/model.h"
#include "umicom/developer_project/build_plan.h"
#include "umicom/developer_project/run_plan.h"


static UmiStatus plan_build(
    const UmiDeveloperProjectModel *model,
    int include_test,
    int include_package,
    UmiDeveloperProjectBuildPlan *out_plan)
{
    (void)include_package;
    return umi_developer_project_build_plan_cargo(
        out_plan,
        model->root,
        0,
        include_test);
}

static UmiStatus plan_run(
    const UmiDeveloperProjectModel *model,
    UmiDeveloperProjectRunPlan *out_plan)
{
    return umi_developer_project_run_plan_cargo(
        out_plan,
        model->root,
        0);
}


const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_cargo(void)
{
    static const UmiDeveloperProjectLanguageProvider provider = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperProjectLanguageProvider),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .provider_id = "developer.provider.cargo",
        .title = "Cargo",
        .primary_language_id = "developer.language.rust",
        .build_system = UMI_DEVELOPER_PROJECT_BUILD_CARGO,
        .priority = 100,
        .built_in = 1,
        .plan_build = plan_build,
        .plan_run = plan_run
    };

    return &provider;
}
