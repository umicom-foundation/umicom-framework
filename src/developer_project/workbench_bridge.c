/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/workbench_bridge.c
 *
 * PURPOSE:
 *   Connect New Project wizard output and canonical project models to the
 *   reusable developer workbench lifecycle configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/workbench_bridge.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the make target identifier operation used by this module and its client
 * applications.
 */
static UmiStatus make_target_identifier(
    const char *source,
    char *destination,
    size_t capacity)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || source[0] == '\0' ||
        destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; source[index] != '\0'; ++index) {
        const unsigned char value = (unsigned char)source[index];

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index + 1U >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        destination[index] =
            (value >= (unsigned char)'A' && value <= (unsigned char)'Z') ||
            (value >= (unsigned char)'a' && value <= (unsigned char)'z') ||
            (value >= (unsigned char)'0' && value <= (unsigned char)'9') ||
            value == (unsigned char)'_'
                ? (char)value
                : '_';
    }

    destination[index] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the developer project request from wizard operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_request_from_wizard(
    const UmiDeveloperWorkbenchProjectWizard *wizard,
    UmiDeveloperProjectGenerationRequest *out_request)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (wizard == NULL || out_request == NULL || !wizard->ready) {
        return UMI_STATUS_INVALID_STATE;
    }

    umi_developer_project_generation_request_init(out_request);

    status = copy_text(
        out_request->template_id,
        sizeof(out_request->template_id),
        wizard->preset_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        /*
         * Application presets and project templates are different catalogues.
         * The explicit mappings below convert familiar New Project choices to
         * concrete starter templates while preserving the original preset IDs.
         */
        return status;
    }

    /*
     * Map application-preset families to concrete starter recipes. Studio can
     * later expose a dedicated project-template picker without changing this
     * Framework bridge.
     */
    if (strcmp(wizard->preset_id, "umicom.preset.console-tool") == 0) {
        (void)snprintf(out_request->template_id,
                       sizeof(out_request->template_id),
                       "%s",
                       "developer.template.c23-console");
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(wizard->preset_id,
                      "umicom.preset.desktop-client") == 0 ||
               strcmp(wizard->preset_id,
                      "umicom.preset.developer-workbench") == 0) {
        (void)snprintf(out_request->template_id,
                       sizeof(out_request->template_id),
                       "%s",
                       "developer.template.thin-desktop-application");
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(wizard->preset_id,
                      "umicom.preset.web-service") == 0) {
        (void)snprintf(out_request->template_id,
                       sizeof(out_request->template_id),
                       "%s",
                       "developer.template.web-service-c23");
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(wizard->preset_id,
                      "umicom.preset.integration-worker") == 0) {
        (void)snprintf(out_request->template_id,
                       sizeof(out_request->template_id),
                       "%s",
                       "developer.template.integration-worker");
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)snprintf(out_request->template_id,
                       sizeof(out_request->template_id),
                       "%s",
                       "developer.template.c23-console");
    }

    status = copy_text(
        out_request->application_name,
        sizeof(out_request->application_name),
        wizard->application_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        out_request->application_id,
        sizeof(out_request->application_id),
        wizard->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        out_request->repository_name,
        sizeof(out_request->repository_name),
        wizard->repository_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = make_target_identifier(
        wizard->repository_name,
        out_request->target_name,
        sizeof(out_request->target_name));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return copy_text(
        out_request->project_root,
        sizeof(out_request->project_root),
        wizard->destination);
}

/*
 * Provide the developer project generate from wizard operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_generate_from_wizard(
    UmiDeveloperProjectService *projects,
    const UmiDeveloperWorkbenchProjectWizard *wizard,
    int dry_run,
    UmiDeveloperProjectGeneratorReport *out_report,
    UmiDeveloperProjectModel *out_model)
{
    UmiDeveloperProjectGenerationRequest request;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projects == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_developer_project_request_from_wizard(
        wizard, &request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    request.dry_run = dry_run != 0;

    return umi_developer_project_service_generate(
        projects,
        &request,
        out_report,
        out_model);
}

/*
 * Provide the developer project model to workbench configuration operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_project_model_to_workbench_configuration(
    const UmiDeveloperProjectModel *model,
    UmiDeveloperWorkbenchConfiguration *out_configuration)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_configuration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_model_validate(model, NULL, 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_developer_workbench_configuration_init(
        out_configuration,
        model->project_id,
        model->display_name);

    (void)snprintf(
        out_configuration->source_root,
        sizeof(out_configuration->source_root),
        "%s",
        model->root);
    (void)snprintf(
        out_configuration->build_directory,
        sizeof(out_configuration->build_directory),
        "%s",
        model->build_directory);
    (void)snprintf(
        out_configuration->preset,
        sizeof(out_configuration->preset),
        "%s",
        model->preset);
    (void)snprintf(
        out_configuration->executable,
        sizeof(out_configuration->executable),
        "%s",
        model->executable);
    (void)snprintf(
        out_configuration->install_prefix,
        sizeof(out_configuration->install_prefix),
        "%s",
        model->install_prefix);

    return UMI_STATUS_OK;
}

/*
 * Provide the developer project workbench adopt model operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_project_workbench_adopt_model(
    UmiDeveloperWorkbench *workbench,
    const UmiDeveloperProjectModel *model,
    int make_active)
{
    UmiDeveloperWorkbenchConfiguration configuration;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_model_to_workbench_configuration(
        model, &configuration);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_developer_workbench_add_configuration(
        workbench,
        &configuration,
        make_active);
}
