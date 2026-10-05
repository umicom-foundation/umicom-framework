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
#include "../base/value_archive_internal.h"

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
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (wizard == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(wizard->application_name, '\0', sizeof(wizard->application_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(wizard->application_id, '\0', sizeof(wizard->application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(wizard->repository_name, '\0', sizeof(wizard->repository_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(wizard->destination, '\0', sizeof(wizard->destination)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(wizard->preset_id, '\0', sizeof(wizard->preset_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(wizard->validation_message, '\0', sizeof(wizard->validation_message)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

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

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDeveloperWorkbenchProjectWizardArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5f79aa18eae82e9f);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->application_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->repository_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->destination)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->preset_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->validation_message)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDeveloperWorkbenchProjectWizardArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->application_name) - 1U +
        8U + sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->application_id) - 1U +
        8U + sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->repository_name) - 1U +
        8U + sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->destination) - 1U +
        8U + sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->preset_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiDeveloperWorkbenchProjectWizard *)0)->validation_message) - 1U +
        8U;
}
static void UmiDeveloperWorkbenchProjectWizardArchiveWrite(UmiArchiveWriter *writer, const UmiDeveloperWorkbenchProjectWizard *value)
{
    UmiArchiveWriteText(writer, value->application_name, sizeof(value->application_name));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->repository_name, sizeof(value->repository_name));
    UmiArchiveWriteText(writer, value->destination, sizeof(value->destination));
    UmiArchiveWriteText(writer, value->preset_id, sizeof(value->preset_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->frontends);
    UmiArchiveWriteSigned(writer, (int64_t)value->initialise_git);
    UmiArchiveWriteSigned(writer, (int64_t)value->create_initial_commit);
    UmiArchiveWriteSigned(writer, (int64_t)value->ready);
    UmiArchiveWriteText(writer, value->validation_message, sizeof(value->validation_message));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiDeveloperWorkbenchProjectWizardArchiveRead(UmiArchiveReader *reader, UmiDeveloperWorkbenchProjectWizard *value)
{
    UmiArchiveReadText(reader, value->application_name, sizeof(value->application_name));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->repository_name, sizeof(value->repository_name));
    UmiArchiveReadText(reader, value->destination, sizeof(value->destination));
    UmiArchiveReadText(reader, value->preset_id, sizeof(value->preset_id));
    value->frontends = (unsigned)UmiArchiveReadUnsigned(reader, UINT_MAX);
    value->initialise_git = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->create_initial_commit = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->ready = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->validation_message, sizeof(value->validation_message));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDeveloperWorkbenchProjectWizardArchiveValidate(const UmiDeveloperWorkbenchProjectWizard *value)
{
    return umi_developer_workbench_project_wizard_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_developer_workbench_project_wizard_archive_encode, umi_developer_workbench_project_wizard_archive_decode,
    UmiDeveloperWorkbenchProjectWizard, UmiDeveloperWorkbenchProjectWizardArchiveSchema, UmiDeveloperWorkbenchProjectWizardArchiveBound, UmiDeveloperWorkbenchProjectWizardArchiveWrite, UmiDeveloperWorkbenchProjectWizardArchiveRead, UmiDeveloperWorkbenchProjectWizardArchiveValidate)
