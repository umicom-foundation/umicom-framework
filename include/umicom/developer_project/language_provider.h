/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/language_provider.h
 *
 * PURPOSE:
 *   Define extensible build/run planning providers behind stable project and
 *   language contracts. Providers may be built-in or supplied by extensions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_LANGUAGE_PROVIDER_H
#define UMICOM_DEVELOPER_PROJECT_LANGUAGE_PROVIDER_H

#include "umicom/developer_project/build_plan.h"
#include "umicom/developer_project/model.h"
#include "umicom/developer_project/run_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiDeveloperProjectPlanBuildFunction)(
    const UmiDeveloperProjectModel *model,
    int include_test,
    int include_package,
    UmiDeveloperProjectBuildPlan *out_plan);

typedef UmiStatus (*UmiDeveloperProjectPlanRunFunction)(
    const UmiDeveloperProjectModel *model,
    UmiDeveloperProjectRunPlan *out_plan);

typedef struct UmiDeveloperProjectLanguageProvider {
    uint32_t structure_size;
    uint32_t api_version;
    const char *provider_id;
    const char *title;
    const char *primary_language_id;
    UmiDeveloperProjectBuildSystem build_system;
    int priority;
    int built_in;
    UmiDeveloperProjectPlanBuildFunction plan_build;
    UmiDeveloperProjectPlanRunFunction plan_run;
} UmiDeveloperProjectLanguageProvider;

UmiStatus umi_developer_project_language_provider_validate(
    const UmiDeveloperProjectLanguageProvider *provider);

#ifdef __cplusplus
}
#endif

#endif
