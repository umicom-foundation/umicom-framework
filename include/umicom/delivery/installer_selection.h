/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/installer_selection.h
 *
 * PURPOSE:
 *   Define the reusable application checkbox model used by graphical and
 *   generated Umicom installers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_INSTALLER_SELECTION_H
#define UMICOM_DELIVERY_INSTALLER_SELECTION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_INSTALLER_SELECTION_MAX_APPLICATIONS 64U
#define UMI_INSTALLER_SELECTION_TEXT_CAPACITY 512U

typedef struct UmiInstallerApplicationDefinition {
    uint32_t structure_size;
    const char *application_id;
    const char *display_name;
    const char *description;
    const char *component_name;
    const char *executable_name;
    uint64_t estimated_size_bytes;
    bool required;
    bool selected_by_default;
    bool available;
} UmiInstallerApplicationDefinition;

typedef struct UmiInstallerApplicationOption {
    char application_id[UMI_DELIVERY_ID_CAPACITY];
    char display_name[UMI_DELIVERY_ID_CAPACITY];
    char description[UMI_INSTALLER_SELECTION_TEXT_CAPACITY];
    char component_name[UMI_DELIVERY_ID_CAPACITY];
    char executable_name[UMI_DELIVERY_PATH_CAPACITY];
    uint64_t estimated_size_bytes;
    bool required;
    bool selected;
    bool available;
} UmiInstallerApplicationOption;

typedef struct UmiInstallerSelection {
    UmiInstallerApplicationOption options[
        UMI_INSTALLER_SELECTION_MAX_APPLICATIONS];
    size_t count;
    uint64_t revision;
} UmiInstallerSelection;

typedef struct UmiInstallerSelectionSnapshot {
    size_t application_count;
    size_t available_count;
    size_t required_count;
    size_t selected_count;
    uint64_t selected_size_bytes;
    uint64_t revision;
} UmiInstallerSelectionSnapshot;

UmiStatus umi_installer_selection_init(UmiInstallerSelection *selection);
UmiStatus umi_installer_selection_add(
    UmiInstallerSelection *selection,
    const UmiInstallerApplicationDefinition *definition);
UmiStatus umi_installer_selection_set_selected(
    UmiInstallerSelection *selection,
    const char *application_id,
    bool selected);
UmiStatus umi_installer_selection_select_all(
    UmiInstallerSelection *selection);
UmiStatus umi_installer_selection_clear_optional(
    UmiInstallerSelection *selection);
UmiStatus umi_installer_selection_at(
    const UmiInstallerSelection *selection,
    size_t index,
    UmiInstallerApplicationOption *out_option);
UmiStatus umi_installer_selection_find(
    const UmiInstallerSelection *selection,
    const char *application_id,
    UmiInstallerApplicationOption *out_option);
UmiStatus umi_installer_selection_snapshot(
    const UmiInstallerSelection *selection,
    UmiInstallerSelectionSnapshot *out_snapshot);
UmiStatus umi_installer_selection_validate(
    const UmiInstallerSelection *selection);

#ifdef __cplusplus
}
#endif

#endif
