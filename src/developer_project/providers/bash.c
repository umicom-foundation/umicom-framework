/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/providers/bash.c
 *
 * PURPOSE:
 *   Implement Bash Script build/run planning through safe Framework argv
 *   plans. No shell interpolation or Studio-specific process logic is used.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/providers/bash.h"

#include "umicom/developer_project/model.h"
#include "umicom/developer_project/build_plan.h"
#include "umicom/developer_project/run_plan.h"


static UmiStatus plan_run(
    const UmiDeveloperProjectModel *model,
    UmiDeveloperProjectRunPlan *out_plan)
{
    if (model->entry_point[0] == '\0') return UMI_STATUS_NOT_FOUND;

    return umi_developer_project_run_plan_interpreter(
        out_plan,
        model->primary_language_id,
        "bash",
        model->entry_point,
        model->root);
}


const UmiDeveloperProjectLanguageProvider *umi_developer_project_provider_bash(void)
{
    static const UmiDeveloperProjectLanguageProvider provider = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperProjectLanguageProvider),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .provider_id = "developer.provider.bash",
        .title = "Bash Script",
        .primary_language_id = "developer.language.bash",
        .build_system = UMI_DEVELOPER_PROJECT_BUILD_SCRIPT,
        .priority = 100,
        .built_in = 1,
        .plan_build = NULL,
        .plan_run = plan_run
    };

    return &provider;
}
