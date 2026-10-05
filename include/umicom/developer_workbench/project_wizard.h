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
#include "umicom/base/value_archive.h"
#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer workbench project wizard data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise developer workbench project wizard from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_workbench_project_wizard_init(
    UmiDeveloperWorkbenchProjectWizard *wizard);

/**
 * Provide the developer workbench project wizard select preset operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_project_wizard_select_preset(
    UmiDeveloperWorkbenchProjectWizard *wizard,
    const char *preset_id);

/**
 * Provide the developer workbench project wizard set identity operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_project_wizard_set_identity(
    UmiDeveloperWorkbenchProjectWizard *wizard,
    const char *application_name,
    const char *application_id,
    const char *repository_name,
    const char *destination);

/**
 * Check that developer workbench project wizard satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_developer_workbench_project_wizard_validate(
    UmiDeveloperWorkbenchProjectWizard *wizard);

/**
 * Provide the developer workbench project wizard preset operation used by this module and
 * its client applications.
 */
const UmiApplicationPresetDefinition *
umi_developer_workbench_project_wizard_preset(
    const UmiDeveloperWorkbenchProjectWizard *wizard);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_developer_workbench_project_wizard_archive_encode(const UmiDeveloperWorkbenchProjectWizard *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_developer_workbench_project_wizard_archive_decode(const void *bytes, size_t byte_count,
    UmiDeveloperWorkbenchProjectWizard *value);

#ifdef __cplusplus
}
#endif

#endif
