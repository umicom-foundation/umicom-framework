/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/project_wizard.h
 *
 * PURPOSE:
 *   Plan a new thin Umicom application from Framework application presets and
 *   feature packs without placing template logic in Umicom Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_PROJECT_WIZARD_H
#define UMICOM_DEVELOPER_WORKBENCH_PROJECT_WIZARD_H

#include "umicom/application/preset.h"
#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperWorkbenchProjectWizard {
    char application_name[UMI_DEVELOPER_WORKBENCH_TITLE_CAPACITY];
    char application_id[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    char repository_name[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    char destination[UMI_DEVELOPER_WORKBENCH_PATH_CAPACITY];
    char preset_id[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    unsigned frontends;
    int initialise_git;
    int create_initial_commit;
    int ready;
    char validation_message[UMI_DEVELOPER_WORKBENCH_TEXT_CAPACITY];
    uint64_t revision;
} UmiDeveloperWorkbenchProjectWizard;

void umi_developer_workbench_project_wizard_init(
    UmiDeveloperWorkbenchProjectWizard *wizard);

UmiStatus umi_developer_workbench_project_wizard_select_preset(
    UmiDeveloperWorkbenchProjectWizard *wizard,
    const char *preset_id);

UmiStatus umi_developer_workbench_project_wizard_set_identity(
    UmiDeveloperWorkbenchProjectWizard *wizard,
    const char *application_name,
    const char *application_id,
    const char *repository_name,
    const char *destination);

UmiStatus umi_developer_workbench_project_wizard_validate(
    UmiDeveloperWorkbenchProjectWizard *wizard);

const UmiApplicationPresetDefinition *
umi_developer_workbench_project_wizard_preset(
    const UmiDeveloperWorkbenchProjectWizard *wizard);

#ifdef __cplusplus
}
#endif

#endif
