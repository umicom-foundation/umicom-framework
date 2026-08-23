/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/service.h
 *
 * PURPOSE:
 *   Compose built-in languages, project templates, generated/imported project
 *   models, generation planning and build/run planning into one reusable service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_SERVICE_H
#define UMICOM_DEVELOPER_PROJECT_SERVICE_H

#include "umicom/developer_project/builtin_languages.h"
#include "umicom/developer_project/builtin_templates.h"
#include "umicom/developer_project/builtin_providers.h"
#include "umicom/developer_project/build_plan.h"
#include "umicom/developer_project/detection.h"
#include "umicom/developer_project/generation_request.h"
#include "umicom/developer_project/generator.h"
#include "umicom/developer_project/model_registry.h"
#include "umicom/developer_project/run_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperProjectService UmiDeveloperProjectService;

typedef struct UmiDeveloperProjectServiceSnapshot {
    size_t language_count;
    size_t template_count;
    size_t project_count;
    char active_project_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    uint64_t revision;
} UmiDeveloperProjectServiceSnapshot;

UmiStatus umi_developer_project_service_create(
    UmiDeveloperProjectService **out_service);

void umi_developer_project_service_destroy(
    UmiDeveloperProjectService *service);

UmiStatus umi_developer_project_service_plan_generation(
    UmiDeveloperProjectService *service,
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectGenerationPlan *out_plan);

UmiStatus umi_developer_project_service_generate(
    UmiDeveloperProjectService *service,
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectGeneratorReport *out_report,
    UmiDeveloperProjectModel *out_model);

UmiStatus umi_developer_project_service_import_indexed(
    UmiDeveloperProjectService *service,
    const UmiFileIndex *file_index,
    const char *project_id,
    const char *display_name,
    const char *root,
    UmiDeveloperProjectModel *out_model);

UmiStatus umi_developer_project_service_plan_build(
    UmiDeveloperProjectService *service,
    const char *project_id,
    int include_test,
    int include_package,
    UmiDeveloperProjectBuildPlan *out_plan);

UmiStatus umi_developer_project_service_plan_run(
    UmiDeveloperProjectService *service,
    const char *project_id,
    UmiDeveloperProjectRunPlan *out_plan);

UmiStatus umi_developer_project_service_snapshot(
    UmiDeveloperProjectService *service,
    UmiDeveloperProjectServiceSnapshot *out_snapshot);

UmiDeveloperProjectLanguageRegistry *
umi_developer_project_service_languages(
    UmiDeveloperProjectService *service);

UmiDeveloperProjectTemplateRegistry *
umi_developer_project_service_templates(
    UmiDeveloperProjectService *service);

UmiDeveloperProjectModelRegistry *
umi_developer_project_service_models(
    UmiDeveloperProjectService *service);

UmiDeveloperProjectProviderRegistry *
umi_developer_project_service_providers(
    UmiDeveloperProjectService *service);

#ifdef __cplusplus
}
#endif

#endif
