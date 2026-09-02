/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/service.h
 *
 * PURPOSE:
 *   Compose built-in languages, project templates, generated/imported project
 *   models, generation planning and build/run planning into one reusable service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the developer project service data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperProjectService UmiDeveloperProjectService;

/**
 * Represent the developer project service snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperProjectServiceSnapshot {
    size_t language_count;
    size_t template_count;
    size_t project_count;
    char active_project_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    uint64_t revision;
} UmiDeveloperProjectServiceSnapshot;

/**
 * Initialise developer project service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_project_service_create(
    UmiDeveloperProjectService **out_service);

/**
 * Release or reset state held by developer project service so the same storage can be
 * reused safely.
 */
void umi_developer_project_service_destroy(
    UmiDeveloperProjectService *service);

/**
 * Provide the developer project service plan generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_project_service_plan_generation(
    UmiDeveloperProjectService *service,
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectGenerationPlan *out_plan);

/**
 * Provide the developer project service generate operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_service_generate(
    UmiDeveloperProjectService *service,
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectGeneratorReport *out_report,
    UmiDeveloperProjectModel *out_model);

/**
 * Provide the developer project service import indexed operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_project_service_import_indexed(
    UmiDeveloperProjectService *service,
    const UmiFileIndex *file_index,
    const char *project_id,
    const char *display_name,
    const char *root,
    UmiDeveloperProjectModel *out_model);

/**
 * Provide the developer project service plan build operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_service_plan_build(
    UmiDeveloperProjectService *service,
    const char *project_id,
    int include_test,
    int include_package,
    UmiDeveloperProjectBuildPlan *out_plan);

/**
 * Perform developer project service plan through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_developer_project_service_plan_run(
    UmiDeveloperProjectService *service,
    const char *project_id,
    UmiDeveloperProjectRunPlan *out_plan);

/**
 * Provide the developer project service snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_service_snapshot(
    UmiDeveloperProjectService *service,
    UmiDeveloperProjectServiceSnapshot *out_snapshot);

/**
 * Provide the developer project service languages operation used by this module and its
 * client applications.
 */
UmiDeveloperProjectLanguageRegistry *
umi_developer_project_service_languages(
    UmiDeveloperProjectService *service);

/**
 * Provide the developer project service templates operation used by this module and its
 * client applications.
 */
UmiDeveloperProjectTemplateRegistry *
umi_developer_project_service_templates(
    UmiDeveloperProjectService *service);

/**
 * Provide the developer project service models operation used by this module and its
 * client applications.
 */
UmiDeveloperProjectModelRegistry *
umi_developer_project_service_models(
    UmiDeveloperProjectService *service);

/**
 * Provide the developer project service providers operation used by this module and its
 * client applications.
 */
UmiDeveloperProjectProviderRegistry *
umi_developer_project_service_providers(
    UmiDeveloperProjectService *service);

#ifdef __cplusplus
}
#endif

#endif
