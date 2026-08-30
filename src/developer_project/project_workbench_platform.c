/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/project_workbench_platform.c
 *
 * PURPOSE:
 *   Implement one Framework-owned project + IDE workbench composition root.
 *
 * ARCHITECTURE:
 *   The UI edits the Framework wizard model. "project.create" then applies the
 *   reviewed plan, registers the new project model and adopts its build/run
 *   configuration into the shared developer workbench. Studio does not need a
 *   separate generator or project-lifecycle implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/project_workbench_platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperProjectWorkbenchPlatform {
    UmiDeveloperWorkbench *workbench;
    UmiDeveloperProjectService *projects;
    UmiDeveloperWorkbenchProjectWizard wizard;
    uint64_t revision;
};

static int always_enabled(void *user_data, const char *argument)
{
    (void)user_data;
    (void)argument;
    return 1;
}

static UmiStatus new_project_action(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProjectWorkbenchPlatform *platform =
        (UmiDeveloperProjectWorkbenchPlatform *)user_data;

    (void)argument;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    umi_developer_workbench_project_wizard_init(&platform->wizard);
    platform->revision += 1U;

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "%s",
            "New Project session initialised.");
    }

    return UMI_STATUS_OK;
}

static int preset_enabled(void *user_data, const char *argument)
{
    (void)user_data;
    return argument != NULL &&
        umi_application_preset_catalogue_find(argument) != NULL;
}

static UmiStatus preset_action(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProjectWorkbenchPlatform *platform =
        (UmiDeveloperProjectWorkbenchPlatform *)user_data;
    UmiStatus status;

    if (platform == NULL || argument == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_workbench_project_wizard_select_preset(
        &platform->wizard,
        argument);

    if (status == UMI_STATUS_OK) {
        platform->revision += 1U;

        if (out_message != NULL && message_capacity > 0U) {
            (void)snprintf(
                out_message,
                message_capacity,
                "Selected project preset: %s",
                argument);
        }
    }

    return status;
}

static int create_enabled(void *user_data, const char *argument)
{
    UmiDeveloperProjectWorkbenchPlatform *platform =
        (UmiDeveloperProjectWorkbenchPlatform *)user_data;

    (void)argument;
    return platform != NULL && platform->wizard.ready;
}

static UmiStatus create_action(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiDeveloperProjectWorkbenchPlatform *platform =
        (UmiDeveloperProjectWorkbenchPlatform *)user_data;
    UmiDeveloperProjectGeneratorReport report;
    UmiDeveloperProjectModel model;
    UmiStatus status;

    (void)argument;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_developer_project_generate_from_wizard(
        platform->projects,
        &platform->wizard,
        0,
        &report,
        &model);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_project_workbench_adopt_model(
        platform->workbench,
        &model,
        1);
    if (status != UMI_STATUS_OK) return status;

    platform->revision += 1U;

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "Created %zu files for %s.",
            report.files_created,
            model.display_name);
    }

    return UMI_STATUS_OK;
}

static UmiStatus bind_project_actions(
    UmiDeveloperProjectWorkbenchPlatform *platform)
{
    UmiStatus status;

    status = umi_developer_workbench_bind_action(
        platform->workbench,
        "project.new",
        new_project_action,
        always_enabled,
        platform);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_workbench_bind_action(
        platform->workbench,
        "project.preset.select",
        preset_action,
        preset_enabled,
        platform);
    if (status != UMI_STATUS_OK) return status;

    return umi_developer_workbench_bind_action(
        platform->workbench,
        "project.create",
        create_action,
        create_enabled,
        platform);
}

UmiStatus umi_developer_project_workbench_platform_create(
    const UmiDeveloperWorkbenchBindings *bindings,
    UmiDeveloperProjectWorkbenchPlatform **out_platform)
{
    UmiDeveloperProjectWorkbenchPlatform *platform;
    UmiStatus status;

    if (bindings == NULL || out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_platform = NULL;
    platform = (UmiDeveloperProjectWorkbenchPlatform *)calloc(
        1U, sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    platform->revision = 1U;
    umi_developer_workbench_project_wizard_init(&platform->wizard);

    status = umi_developer_workbench_create(
        bindings, &platform->workbench);
    if (status == UMI_STATUS_OK) {
        status = umi_developer_project_service_create(
            &platform->projects);
    }
    if (status == UMI_STATUS_OK) {
        status = bind_project_actions(platform);
    }

    if (status != UMI_STATUS_OK) {
        umi_developer_project_workbench_platform_destroy(platform);
        return status;
    }

    *out_platform = platform;
    return UMI_STATUS_OK;
}

void umi_developer_project_workbench_platform_destroy(
    UmiDeveloperProjectWorkbenchPlatform *platform)
{
    if (platform == NULL) return;

    umi_developer_project_service_destroy(platform->projects);
    umi_developer_workbench_destroy(platform->workbench);
    free(platform);
}

UmiDeveloperWorkbench *
umi_developer_project_workbench_platform_workbench(
    UmiDeveloperProjectWorkbenchPlatform *platform)
{
    return platform != NULL ? platform->workbench : NULL;
}

UmiDeveloperProjectService *
umi_developer_project_workbench_platform_projects(
    UmiDeveloperProjectWorkbenchPlatform *platform)
{
    return platform != NULL ? platform->projects : NULL;
}

UmiDeveloperWorkbenchProjectWizard *
umi_developer_project_workbench_platform_wizard(
    UmiDeveloperProjectWorkbenchPlatform *platform)
{
    return platform != NULL ? &platform->wizard : NULL;
}

UmiStatus umi_developer_project_workbench_platform_snapshot(
    UmiDeveloperProjectWorkbenchPlatform *platform,
    UmiDeveloperProjectWorkbenchPlatformSnapshot *out_snapshot)
{
    UmiStatus status;

    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));

    status = umi_developer_workbench_snapshot(
        platform->workbench,
        &out_snapshot->workbench);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_project_service_snapshot(
        platform->projects,
        &out_snapshot->projects);
    if (status != UMI_STATUS_OK) return status;

    out_snapshot->new_project_ready = platform->wizard.ready;
    (void)snprintf(
        out_snapshot->selected_application_preset,
        sizeof(out_snapshot->selected_application_preset),
        "%s",
        platform->wizard.preset_id);
    out_snapshot->revision = platform->revision;
    return UMI_STATUS_OK;
}
