/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/parameter_information.h
 *
 * PURPOSE:
 *   Define reusable callable-parameter metadata and active-parameter state
 *   independently from any language server, parser or graphical toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_PARAMETER_INFORMATION_H
#define UMICOM_EDITOR_PARAMETER_INFORMATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_PARAMETER_INFORMATION_API_VERSION 1U
#define UMI_EDITOR_PARAMETER_SIGNATURE_ID_CAPACITY 128U
#define UMI_EDITOR_PARAMETER_LABEL_CAPACITY 256U
#define UMI_EDITOR_PARAMETER_TYPE_CAPACITY 256U
#define UMI_EDITOR_PARAMETER_DOCUMENTATION_CAPACITY 2048U
#define UMI_EDITOR_PARAMETER_DEFAULT_CAPACITY 256U

/**
 * Represent the editor parameter information item data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorParameterInformationItem {
    uint32_t struct_size;
    uint32_t api_version;
    char signature_id[UMI_EDITOR_PARAMETER_SIGNATURE_ID_CAPACITY];
    char label[UMI_EDITOR_PARAMETER_LABEL_CAPACITY];
    char type_name[UMI_EDITOR_PARAMETER_TYPE_CAPACITY];
    char documentation[UMI_EDITOR_PARAMETER_DOCUMENTATION_CAPACITY];
    char default_value[UMI_EDITOR_PARAMETER_DEFAULT_CAPACITY];
    size_t ordinal;
    size_t label_start;
    size_t label_end;
    int required;
    int variadic;
    int deprecated;
} UmiEditorParameterInformationItem;

/**
 * Represent the editor parameter information snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorParameterInformationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t parameter_count;
    size_t signature_count;
    size_t required_parameter_count;
    size_t deprecated_parameter_count;
    size_t active_ordinal;
    uint64_t revision;
    char active_signature_id[UMI_EDITOR_PARAMETER_SIGNATURE_ID_CAPACITY];
    int has_active_parameter;
} UmiEditorParameterInformationSnapshot;

/**
 * Represent the editor parameter information data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorParameterInformation UmiEditorParameterInformation;

/**
 * Initialise editor parameter information from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_parameter_information_create(
    UmiEditorParameterInformation **out_information);
/**
 * Release or reset state held by editor parameter information so the same storage can be
 * reused safely.
 */
void umi_editor_parameter_information_destroy(
    UmiEditorParameterInformation *information);
/**
 * Release or reset state held by editor parameter information so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_parameter_information_clear(
    UmiEditorParameterInformation *information);
/**
 * Provide the editor parameter information upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_parameter_information_upsert(
    UmiEditorParameterInformation *information,
    const UmiEditorParameterInformationItem *item);
/**
 * Remove editor parameter information while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_parameter_information_remove(
    UmiEditorParameterInformation *information,
    const char *signature_id,
    size_t ordinal);
/**
 * Provide the editor parameter information remove signature operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_parameter_information_remove_signature(
    UmiEditorParameterInformation *information,
    const char *signature_id);
/**
 * Provide the editor parameter information sort operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_parameter_information_sort(
    UmiEditorParameterInformation *information);
/**
 * Provide the editor parameter information set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_parameter_information_set_active(
    UmiEditorParameterInformation *information,
    const char *signature_id,
    size_t ordinal);
/**
 * Provide the editor parameter information clear active operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_parameter_information_clear_active(
    UmiEditorParameterInformation *information);
/**
 * Find editor parameter information while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_parameter_information_at(
    const UmiEditorParameterInformation *information,
    size_t index,
    UmiEditorParameterInformationItem *out_item);
/**
 * Find editor parameter information for signature while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_editor_parameter_information_for_signature_at(
    const UmiEditorParameterInformation *information,
    const char *signature_id,
    size_t signature_index,
    UmiEditorParameterInformationItem *out_item);
/**
 * Provide the editor parameter information active operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_parameter_information_active(
    const UmiEditorParameterInformation *information,
    UmiEditorParameterInformationItem *out_item);
/**
 * Provide the editor parameter information snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_parameter_information_snapshot(
    const UmiEditorParameterInformation *information,
    UmiEditorParameterInformationSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor parameter information without
 * changing their state.
 */
size_t umi_editor_parameter_information_count(
    const UmiEditorParameterInformation *information);
/**
 * Return the number of records represented by editor parameter information signature
 * without changing their state.
 */
size_t umi_editor_parameter_information_signature_count(
    const UmiEditorParameterInformation *information,
    const char *signature_id);
/**
 * Provide the editor parameter information revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_parameter_information_revision(
    const UmiEditorParameterInformation *information);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_PARAMETER_INFORMATION_H */
