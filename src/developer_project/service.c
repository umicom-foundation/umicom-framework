/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/service.c
 *
 * PURPOSE:
 *   Implement the reusable Framework developer-project composition root.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperProjectService {
    UmiDeveloperProjectLanguageRegistry *languages;
    UmiDeveloperProjectTemplateRegistry *templates;
    UmiDeveloperProjectModelRegistry *models;
    UmiDeveloperProjectProviderRegistry *providers;
    uint64_t revision;
};

UmiStatus umi_developer_project_service_create(
    UmiDeveloperProjectService **out_service)
{
    UmiDeveloperProjectService *service;
    UmiStatus status = UMI_STATUS_OK;

    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;

    service = (UmiDeveloperProjectService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    service->revision = 1U;

    if (status == UMI_STATUS_OK) {
        status = umi_developer_project_language_registry_create(
            &service->languages);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_project_template_registry_create(
            &service->templates);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_project_model_registry_create(
            &service->models);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_project_provider_registry_create(
            &service->providers);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_project_builtin_languages_register(
            service->languages);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_project_builtin_templates_register(
            service->templates);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_developer_project_builtin_providers_register(
            service->providers);
    }

    if (status != UMI_STATUS_OK) {
        umi_developer_project_service_destroy(service);
        return status;
    }

    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_developer_project_service_destroy(
    UmiDeveloperProjectService *service)
{
    if (service == NULL) return;

    umi_developer_project_provider_registry_destroy(service->providers);
    umi_developer_project_model_registry_destroy(service->models);
    umi_developer_project_template_registry_destroy(service->templates);
    umi_developer_project_language_registry_destroy(service->languages);
    free(service);
}

UmiStatus umi_developer_project_service_plan_generation(
    UmiDeveloperProjectService *service,
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectGenerationPlan *out_plan)
{
    const UmiDeveloperProjectTemplate *project_template;
    UmiDeveloperProjectVariableSet variables;
    UmiStatus status;

    if (service == NULL || request == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_generation_request_validate(
        request, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;

    project_template = umi_developer_project_template_registry_find(
        service->templates, request->template_id);
    if (project_template == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_developer_project_generation_request_variables(
        request, &variables);
    if (status != UMI_STATUS_OK) return status;

    return umi_developer_project_generation_plan_build(
        project_template,
        &variables,
        request->project_root,
        out_plan);
}

UmiStatus umi_developer_project_service_generate(
    UmiDeveloperProjectService *service,
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectGeneratorReport *out_report,
    UmiDeveloperProjectModel *out_model)
{
    const UmiDeveloperProjectTemplate *project_template;
    UmiDeveloperProjectGenerationPlan plan;
    UmiDeveloperProjectGeneratorRequest generator_request;
    UmiDeveloperProjectModel model;
    size_t index;
    UmiStatus status;

    if (service == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_service_plan_generation(
        service, request, &plan);
    if (status != UMI_STATUS_OK) return status;

    project_template = umi_developer_project_template_registry_find(
        service->templates, request->template_id);
    if (project_template == NULL) return UMI_STATUS_NOT_FOUND;

    (void)memset(&generator_request, 0, sizeof(generator_request));
    generator_request.plan = &plan;
    generator_request.dry_run = request->dry_run;
    generator_request.overwrite_existing_files =
        request->overwrite_existing_files;

    status = umi_developer_project_generator_apply(
        &generator_request, out_report);
    if (status != UMI_STATUS_OK) return status;

    umi_developer_project_model_init(
        &model,
        request->application_id,
        request->application_name);
    (void)snprintf(model.root, sizeof(model.root), "%s", request->project_root);
    (void)snprintf(
        model.template_id,
        sizeof(model.template_id),
        "%s",
        project_template->template_id);
    (void)snprintf(
        model.primary_language_id,
        sizeof(model.primary_language_id),
        "%s",
        project_template->primary_language_id);
    model.kind = project_template->kind;
    model.build_system = project_template->build_system;
    model.generated = 1;

    /*
     * Populate immediately useful launch/staging defaults for generated
     * starters. They remain editable project-model state, not hard-coded Studio
     * assumptions.
     */
    if (project_template->build_system ==
            UMI_DEVELOPER_PROJECT_BUILD_SCRIPT &&
        strcmp(project_template->primary_language_id,
               "developer.language.bash") == 0) {
        (void)snprintf(
            model.entry_point,
            sizeof(model.entry_point),
            "bin/%s.sh",
            request->target_name);
    } else if (project_template->build_system ==
                   UMI_DEVELOPER_PROJECT_BUILD_SCRIPT &&
               strcmp(project_template->primary_language_id,
                      "developer.language.cpython") == 0) {
        (void)snprintf(
            model.entry_point,
            sizeof(model.entry_point),
            "src/%s.py",
            request->target_name);
    }

    for (index = 0U;
         index < project_template->secondary_language_count;
         ++index) {
        status = umi_developer_project_model_add_language(
            &model,
            project_template->secondary_language_ids[index]);
        if (status != UMI_STATUS_OK) return status;
    }

    if (project_template->build_system ==
        UMI_DEVELOPER_PROJECT_BUILD_CMAKE) {
        (void)snprintf(
            model.preset,
            sizeof(model.preset),
            "%s",
            "windows-ucrt64-debug");
        (void)snprintf(
            model.build_directory,
            sizeof(model.build_directory),
            "%s",
            "build/windows-ucrt64-debug");
        (void)snprintf(
            model.install_prefix,
            sizeof(model.install_prefix),
            "%s",
            "install/windows-ucrt64-debug");

        if (project_template->kind == UMI_DEVELOPER_PROJECT_EXECUTABLE) {
            (void)snprintf(
                model.executable,
                sizeof(model.executable),
                "build/windows-ucrt64-debug/bin/%s.exe",
                request->target_name);
        }
    }

    if (!request->dry_run) {
        status = umi_developer_project_model_registry_upsert(
            service->models, &model);
        if (status != UMI_STATUS_OK) return status;
        service->revision += 1U;
    }

    if (out_model != NULL) *out_model = model;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_service_import_indexed(
    UmiDeveloperProjectService *service,
    const UmiFileIndex *file_index,
    const char *project_id,
    const char *display_name,
    const char *root,
    UmiDeveloperProjectModel *out_model)
{
    UmiDeveloperProjectDetectionReport report;
    UmiDeveloperProjectModel model;
    UmiStatus status;

    if (service == NULL || file_index == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_detect(
        file_index, service->languages, &report);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_project_detection_to_model(
        &report,
        project_id,
        display_name,
        root,
        &model);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_project_model_registry_upsert(
        service->models, &model);
    if (status != UMI_STATUS_OK) return status;

    service->revision += 1U;
    if (out_model != NULL) *out_model = model;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_service_plan_build(
    UmiDeveloperProjectService *service,
    const char *project_id,
    int include_test,
    int include_package,
    UmiDeveloperProjectBuildPlan *out_plan)
{
    UmiDeveloperProjectModel model;
    UmiStatus status;

    if (service == NULL || project_id == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_model_registry_find(
        service->models, project_id, &model);
    if (status != UMI_STATUS_OK) return status;

    {
        const UmiDeveloperProjectLanguageProvider *provider =
            umi_developer_project_provider_registry_select(
                service->providers, &model);

        if (provider == NULL || provider->plan_build == NULL) {
            return UMI_STATUS_NOT_IMPLEMENTED;
        }

        return provider->plan_build(
            &model,
            include_test,
            include_package,
            out_plan);
    }
}

UmiStatus umi_developer_project_service_plan_run(
    UmiDeveloperProjectService *service,
    const char *project_id,
    UmiDeveloperProjectRunPlan *out_plan)
{
    UmiDeveloperProjectModel model;
    const UmiDeveloperProjectLanguageProvider *provider;
    UmiStatus status;

    if (service == NULL || project_id == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_model_registry_find(
        service->models, project_id, &model);
    if (status != UMI_STATUS_OK) return status;

    provider = umi_developer_project_provider_registry_select(
        service->providers, &model);
    if (provider == NULL || provider->plan_run == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return provider->plan_run(&model, out_plan);
}

UmiStatus umi_developer_project_service_snapshot(
    UmiDeveloperProjectService *service,
    UmiDeveloperProjectServiceSnapshot *out_snapshot)
{
    UmiDeveloperProjectModel active;

    if (service == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->language_count =
        umi_developer_project_language_registry_count(service->languages);
    out_snapshot->template_count =
        umi_developer_project_template_registry_count(service->templates);
    out_snapshot->project_count =
        umi_developer_project_model_registry_count(service->models);

    if (umi_developer_project_model_registry_active(
            service->models, &active) == UMI_STATUS_OK) {
        (void)snprintf(
            out_snapshot->active_project_id,
            sizeof(out_snapshot->active_project_id),
            "%s",
            active.project_id);
    }

    out_snapshot->revision = service->revision;
    return UMI_STATUS_OK;
}

UmiDeveloperProjectLanguageRegistry *
umi_developer_project_service_languages(
    UmiDeveloperProjectService *service)
{
    return service != NULL ? service->languages : NULL;
}

UmiDeveloperProjectTemplateRegistry *
umi_developer_project_service_templates(
    UmiDeveloperProjectService *service)
{
    return service != NULL ? service->templates : NULL;
}

UmiDeveloperProjectModelRegistry *
umi_developer_project_service_models(
    UmiDeveloperProjectService *service)
{
    return service != NULL ? service->models : NULL;
}

UmiDeveloperProjectProviderRegistry *
umi_developer_project_service_providers(
    UmiDeveloperProjectService *service)
{
    return service != NULL ? service->providers : NULL;
}
