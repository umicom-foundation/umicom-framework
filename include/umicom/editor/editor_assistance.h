/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/editor_assistance.h
 *
 * PURPOSE:
 *   Coordinate Framework-owned hover, signature-help, parameter-information
 *   and quick-documentation services for every Umicom application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_EDITOR_ASSISTANCE_H
#define UMICOM_EDITOR_EDITOR_ASSISTANCE_H

#include "umicom/editor/hover_model.h"
#include "umicom/editor/parameter_information.h"
#include "umicom/editor/quick_documentation.h"
#include "umicom/editor/signature_help_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_ASSISTANCE_API_VERSION 1U

typedef enum UmiEditorAssistanceFeature {
    UMI_EDITOR_ASSISTANCE_HOVER = 1,
    UMI_EDITOR_ASSISTANCE_SIGNATURE_HELP = 2,
    UMI_EDITOR_ASSISTANCE_PARAMETER_INFORMATION = 3,
    UMI_EDITOR_ASSISTANCE_QUICK_DOCUMENTATION = 4
} UmiEditorAssistanceFeature;

typedef struct UmiEditorAssistanceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t hover_section_count;
    size_t signature_count;
    size_t parameter_count;
    size_t documentation_section_count;
    uint64_t revision;
    int hover_enabled;
    int signature_help_enabled;
    int parameter_information_enabled;
    int quick_documentation_enabled;
    int hover_pinned;
    int signature_help_presented;
    int documentation_pinned;
    int has_active_signature;
    int has_active_parameter;
} UmiEditorAssistanceSnapshot;

typedef struct UmiEditorAssistance UmiEditorAssistance;

UmiStatus umi_editor_assistance_create(
    UmiEditorAssistance **out_assistance);
void umi_editor_assistance_destroy(UmiEditorAssistance *assistance);
UmiStatus umi_editor_assistance_set_feature_enabled(
    UmiEditorAssistance *assistance,
    UmiEditorAssistanceFeature feature,
    int enabled);
int umi_editor_assistance_feature_enabled(
    const UmiEditorAssistance *assistance,
    UmiEditorAssistanceFeature feature);
UmiEditorHoverModel *umi_editor_assistance_hover(
    UmiEditorAssistance *assistance);
UmiEditorSignatureHelpModel *umi_editor_assistance_signatures(
    UmiEditorAssistance *assistance);
UmiEditorParameterInformation *umi_editor_assistance_parameters(
    UmiEditorAssistance *assistance);
UmiEditorQuickDocumentation *umi_editor_assistance_documentation(
    UmiEditorAssistance *assistance);
UmiStatus umi_editor_assistance_activate_signature(
    UmiEditorAssistance *assistance,
    const char *signature_id);
UmiStatus umi_editor_assistance_select_next_signature(
    UmiEditorAssistance *assistance,
    int wrap);
UmiStatus umi_editor_assistance_select_previous_signature(
    UmiEditorAssistance *assistance,
    int wrap);
UmiStatus umi_editor_assistance_activate_parameter(
    UmiEditorAssistance *assistance,
    size_t ordinal);
UmiStatus umi_editor_assistance_snapshot(
    const UmiEditorAssistance *assistance,
    UmiEditorAssistanceSnapshot *out_snapshot);
uint64_t umi_editor_assistance_revision(
    const UmiEditorAssistance *assistance);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_EDITOR_ASSISTANCE_H */
