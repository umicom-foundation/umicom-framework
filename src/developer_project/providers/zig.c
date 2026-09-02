/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/providers/zig.c
 *
 * PURPOSE:
 *   Implement Zig Build build/run planning through safe Framework argv
 *   plans. No shell interpolation or Studio-specific process logic is used.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/providers/zig.h"

#include "umicom/developer_project/model.h"
#include "umicom/developer_project/build_plan.h"
#include "umicom/developer_project/run_plan.h"


/* Provide the plan build operation used by this module and its client applications. */
static UmiStatus plan_build(
    const UmiDeveloperProjectModel *model,
    int include_test,
    int include_package,
    UmiDeveloperProjectBuildPlan *out_plan)
{
    (void)include_package;
    return umi_developer_project_build_plan_zig(
        out_plan,
        model->root,
        include_test);
}

/*
 * Perform plan through the module contract so client applications do not duplicate its
 * policy.
 */
static UmiStatus plan_run(
    const UmiDeveloperProjectModel *model,
    UmiDeveloperProjectRunPlan *out_plan)
{
    return umi_developer_project_run_plan_zig(out_plan, model->root);
}


/*
 * Provide the developer project provider zig operation used by this module and its client
 * applications.
 */
const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_zig(void)
{
    static const UmiDeveloperProjectLanguageProvider provider = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperProjectLanguageProvider),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .provider_id = "developer.provider.zig",
        .title = "Zig Build",
        .primary_language_id = "developer.language.zig",
        .build_system = UMI_DEVELOPER_PROJECT_BUILD_ZIG,
        .priority = 100,
        .built_in = 1,
        .plan_build = plan_build,
        .plan_run = plan_run
    };

    return &provider;
}
