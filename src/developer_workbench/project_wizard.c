/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/project_wizard.c
 *
 * PURPOSE:
 *   Implement Framework-owned project-wizard planning over application presets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/project_wizard.h"

#include <ctype.h>
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
 * Provide the valid application id operation used by this module and its client
 * applications.
 */
static int valid_application_id(const char *text)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || text[0] == '\0' ||
        !islower((unsigned char)text[0])) {
        return 0;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; text[index] != '\0'; ++index) {
        const unsigned char value = (unsigned char)text[index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (!islower(value) && !isdigit(value) &&
            value != '.' && value != '-') {
            return 0;
        }
    }

    return 1;
}

/*
 * Initialise developer workbench project wizard from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_workbench_project_wizard_init(
    UmiDeveloperWorkbenchProjectWizard *wizard)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (wizard == NULL) return;

    (void)memset(wizard, 0, sizeof(*wizard));
    wizard->initialise_git = 1;
    wizard->create_initial_commit = 1;
    wizard->revision = 1U;
}

/*
 * Provide the developer workbench project wizard select preset operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_project_wizard_select_preset(
    UmiDeveloperWorkbenchProjectWizard *wizard,
    const char *preset_id)
{
    const UmiApplicationPresetDefinition *preset;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (wizard == NULL || preset_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    preset = umi_application_preset_catalogue_find(preset_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preset == NULL) return UMI_STATUS_NOT_FOUND;

    status = copy_text(
        wizard->preset_id, sizeof(wizard->preset_id), preset_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    wizard->frontends = preset->recommended_frontends;
    wizard->ready = 0;
    wizard->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer workbench project wizard set identity operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_project_wizard_set_identity(
    UmiDeveloperWorkbenchProjectWizard *wizard,
    const char *application_name,
    const char *application_id,
    const char *repository_name,
    const char *destination)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (wizard == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = copy_text(
        wizard->application_name,
        sizeof(wizard->application_name),
        application_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        wizard->application_id,
        sizeof(wizard->application_id),
        application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        wizard->repository_name,
        sizeof(wizard->repository_name),
        repository_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        wizard->destination,
        sizeof(wizard->destination),
        destination);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    wizard->ready = 0;
    wizard->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Check that developer workbench project wizard satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_developer_workbench_project_wizard_validate(
    UmiDeveloperWorkbenchProjectWizard *wizard)
{
    const UmiApplicationPresetDefinition *preset;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (wizard == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    wizard->ready = 0;

    /* Apply this branch only when its contract condition is satisfied. */
    if (wizard->application_name[0] == '\0' ||
        !valid_application_id(wizard->application_id) ||
        wizard->repository_name[0] == '\0' ||
        wizard->destination[0] == '\0' ||
        wizard->preset_id[0] == '\0') {
        (void)snprintf(
            wizard->validation_message,
            sizeof(wizard->validation_message),
            "%s",
            "Name, application ID, repository, destination and preset are required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    preset = umi_application_preset_catalogue_find(wizard->preset_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preset == NULL) {
        (void)snprintf(
            wizard->validation_message,
            sizeof(wizard->validation_message),
            "%s",
            "Selected Framework application preset was not found.");
        return UMI_STATUS_NOT_FOUND;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_application_preset_validate(preset) != UMI_STATUS_OK) {
        (void)snprintf(
            wizard->validation_message,
            sizeof(wizard->validation_message),
            "%s",
            "Selected Framework application preset is not valid.");
        return UMI_STATUS_INVALID_STATE;
    }

    wizard->ready = 1;
    (void)snprintf(
        wizard->validation_message,
        sizeof(wizard->validation_message),
        "%s",
        "Project plan is ready for Framework repository scaffolding.");
    wizard->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer workbench project wizard preset operation used by this module and
 * its client applications.
 */
const UmiApplicationPresetDefinition *
umi_developer_workbench_project_wizard_preset(
    const UmiDeveloperWorkbenchProjectWizard *wizard)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (wizard == NULL || wizard->preset_id[0] == '\0') return NULL;
    return umi_application_preset_catalogue_find(wizard->preset_id);
}
