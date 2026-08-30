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

typedef struct UmiEditorParameterInformation UmiEditorParameterInformation;

UmiStatus umi_editor_parameter_information_create(
    UmiEditorParameterInformation **out_information);
void umi_editor_parameter_information_destroy(
    UmiEditorParameterInformation *information);
UmiStatus umi_editor_parameter_information_clear(
    UmiEditorParameterInformation *information);
UmiStatus umi_editor_parameter_information_upsert(
    UmiEditorParameterInformation *information,
    const UmiEditorParameterInformationItem *item);
UmiStatus umi_editor_parameter_information_remove(
    UmiEditorParameterInformation *information,
    const char *signature_id,
    size_t ordinal);
UmiStatus umi_editor_parameter_information_remove_signature(
    UmiEditorParameterInformation *information,
    const char *signature_id);
UmiStatus umi_editor_parameter_information_sort(
    UmiEditorParameterInformation *information);
UmiStatus umi_editor_parameter_information_set_active(
    UmiEditorParameterInformation *information,
    const char *signature_id,
    size_t ordinal);
UmiStatus umi_editor_parameter_information_clear_active(
    UmiEditorParameterInformation *information);
UmiStatus umi_editor_parameter_information_at(
    const UmiEditorParameterInformation *information,
    size_t index,
    UmiEditorParameterInformationItem *out_item);
UmiStatus umi_editor_parameter_information_for_signature_at(
    const UmiEditorParameterInformation *information,
    const char *signature_id,
    size_t signature_index,
    UmiEditorParameterInformationItem *out_item);
UmiStatus umi_editor_parameter_information_active(
    const UmiEditorParameterInformation *information,
    UmiEditorParameterInformationItem *out_item);
UmiStatus umi_editor_parameter_information_snapshot(
    const UmiEditorParameterInformation *information,
    UmiEditorParameterInformationSnapshot *out_snapshot);
size_t umi_editor_parameter_information_count(
    const UmiEditorParameterInformation *information);
size_t umi_editor_parameter_information_signature_count(
    const UmiEditorParameterInformation *information,
    const char *signature_id);
uint64_t umi_editor_parameter_information_revision(
    const UmiEditorParameterInformation *information);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_PARAMETER_INFORMATION_H */
