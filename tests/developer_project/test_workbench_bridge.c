/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_workbench_bridge.c
 *
 * PURPOSE:
 *   Verify application presets map into concrete project generation requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>

#include "umicom/developer_project/workbench_bridge.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/developer_workbench/project_wizard.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDeveloperWorkbenchProjectWizardTransferEqual(const UmiDeveloperWorkbenchProjectWizard *a, const UmiDeveloperWorkbenchProjectWizard *b)
{
    return strcmp(a->application_name, b->application_name) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->repository_name, b->repository_name) == 0 &&
        strcmp(a->destination, b->destination) == 0 &&
        strcmp(a->preset_id, b->preset_id) == 0 &&
        a->frontends == b->frontends &&
        a->initialise_git == b->initialise_git &&
        a->create_initial_commit == b->create_initial_commit &&
        a->ready == b->ready &&
        strcmp(a->validation_message, b->validation_message) == 0 &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDeveloperWorkbenchProjectWizardTransferTails(UmiDeveloperWorkbenchProjectWizard *value)
{
    (void)value;
    {
        size_t used = strlen(value->application_name) + 1U;
        memset(value->application_name + used, 0xa5, sizeof(value->application_name) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->repository_name) + 1U;
        memset(value->repository_name + used, 0xa5, sizeof(value->repository_name) - used);
    }
    {
        size_t used = strlen(value->destination) + 1U;
        memset(value->destination + used, 0xa5, sizeof(value->destination) - used);
    }
    {
        size_t used = strlen(value->preset_id) + 1U;
        memset(value->preset_id + used, 0xa5, sizeof(value->preset_id) - used);
    }
    {
        size_t used = strlen(value->validation_message) + 1U;
        memset(value->validation_message + used, 0xa5, sizeof(value->validation_message) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDeveloperWorkbenchProjectWizardTransferMalformed(const UmiDeveloperWorkbenchProjectWizard *sample)
{
    (void)sample;
    {
        UmiDeveloperWorkbenchProjectWizard invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_name, 'x', sizeof(invalid.application_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_developer_workbench_project_wizard_validate(&invalid) != UMI_STATUS_OK) ||
            umi_developer_workbench_project_wizard_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeveloperWorkbenchProjectWizard invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_developer_workbench_project_wizard_validate(&invalid) != UMI_STATUS_OK) ||
            umi_developer_workbench_project_wizard_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeveloperWorkbenchProjectWizard invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.repository_name, 'x', sizeof(invalid.repository_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_developer_workbench_project_wizard_validate(&invalid) != UMI_STATUS_OK) ||
            umi_developer_workbench_project_wizard_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated repository_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeveloperWorkbenchProjectWizard invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.destination, 'x', sizeof(invalid.destination));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_developer_workbench_project_wizard_validate(&invalid) != UMI_STATUS_OK) ||
            umi_developer_workbench_project_wizard_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated destination was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeveloperWorkbenchProjectWizard invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.preset_id, 'x', sizeof(invalid.preset_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_developer_workbench_project_wizard_validate(&invalid) != UMI_STATUS_OK) ||
            umi_developer_workbench_project_wizard_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated preset_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeveloperWorkbenchProjectWizard invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.validation_message, 'x', sizeof(invalid.validation_message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_developer_workbench_project_wizard_validate(&invalid) != UMI_STATUS_OK) ||
            umi_developer_workbench_project_wizard_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated validation_message was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDeveloperWorkbenchProjectWizardTransferCases, UmiDeveloperWorkbenchProjectWizard,
    umi_developer_workbench_project_wizard_archive_encode, umi_developer_workbench_project_wizard_archive_decode,
    UmiDeveloperWorkbenchProjectWizardTransferEqual, UmiDeveloperWorkbenchProjectWizardTransferTails, UmiDeveloperWorkbenchProjectWizardTransferMalformed)

int main(void)
{
    UmiDeveloperWorkbenchProjectWizard wizard;
    UmiDeveloperProjectGenerationRequest request;

    umi_developer_workbench_project_wizard_init(&wizard);
    assert(umi_developer_workbench_project_wizard_select_preset(
        &wizard,
        "umicom.preset.developer-workbench") == UMI_STATUS_OK);
    assert(umi_developer_workbench_project_wizard_set_identity(
        &wizard,
        "Example Studio",
        "org.umicom.example-studio",
        "example-studio",
        "C:/work/example-studio") == UMI_STATUS_OK);
    assert(umi_developer_workbench_project_wizard_validate(&wizard) ==
           UMI_STATUS_OK);
    if (UmiDeveloperWorkbenchProjectWizardTransferCases(&wizard) != 0) return 1;


    assert(umi_developer_project_request_from_wizard(
        &wizard, &request) == UMI_STATUS_OK);
    assert(strcmp(
        request.template_id,
        "developer.template.thin-desktop-application") == 0);
    assert(strcmp(request.target_name, "example_studio") == 0);
    return 0;
}
