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

/**
 * Represent the installer application definition data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the installer application option data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the installer selection data shared with callers of this public contract.
 */
typedef struct UmiInstallerSelection {
    UmiInstallerApplicationOption options[
        UMI_INSTALLER_SELECTION_MAX_APPLICATIONS];
    size_t count;
    uint64_t revision;
} UmiInstallerSelection;

/**
 * Represent the installer selection snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiInstallerSelectionSnapshot {
    size_t application_count;
    size_t available_count;
    size_t required_count;
    size_t selected_count;
    uint64_t selected_size_bytes;
    uint64_t revision;
} UmiInstallerSelectionSnapshot;

/**
 * Initialise installer selection from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_installer_selection_init(UmiInstallerSelection *selection);
/**
 * Add installer selection only after its inputs and available capacity have been checked.
 */
UmiStatus umi_installer_selection_add(
    UmiInstallerSelection *selection,
    const UmiInstallerApplicationDefinition *definition);
/**
 * Find installer selection set while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_installer_selection_set_selected(
    UmiInstallerSelection *selection,
    const char *application_id,
    bool selected);
/**
 * Provide the installer selection select all operation used by this module and its client
 * applications.
 */
UmiStatus umi_installer_selection_select_all(
    UmiInstallerSelection *selection);
/**
 * Provide the installer selection clear optional operation used by this module and its
 * client applications.
 */
UmiStatus umi_installer_selection_clear_optional(
    UmiInstallerSelection *selection);
/**
 * Find installer selection while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_installer_selection_at(
    const UmiInstallerSelection *selection,
    size_t index,
    UmiInstallerApplicationOption *out_option);
/**
 * Find installer selection while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_installer_selection_find(
    const UmiInstallerSelection *selection,
    const char *application_id,
    UmiInstallerApplicationOption *out_option);
/**
 * Provide the installer selection snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_installer_selection_snapshot(
    const UmiInstallerSelection *selection,
    UmiInstallerSelectionSnapshot *out_snapshot);
/**
 * Check that installer selection satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_installer_selection_validate(
    const UmiInstallerSelection *selection);

#ifdef __cplusplus
}
#endif

#endif
