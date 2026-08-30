/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/editor_assistance.c
 *
 * PURPOSE:
 *   Own and coordinate professional editor-assistance services while keeping
 *   application shells and graphical adapters free of reusable logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/editor_assistance.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorAssistance {
    UmiEditorHoverModel *hover;
    UmiEditorSignatureHelpModel *signatures;
    UmiEditorParameterInformation *parameters;
    UmiEditorQuickDocumentation *documentation;
    uint64_t revision;
    int hover_enabled;
    int signature_help_enabled;
    int parameter_information_enabled;
    int quick_documentation_enabled;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int valid_feature(UmiEditorAssistanceFeature feature)
{
    return feature >= UMI_EDITOR_ASSISTANCE_HOVER &&
           feature <= UMI_EDITOR_ASSISTANCE_QUICK_DOCUMENTATION;
}

static uint64_t add_revision(uint64_t left, uint64_t right)
{
    uint64_t mixed;

    if (UINT64_MAX - left < right) {
        mixed = left ^ right;
        return mixed == UINT64_MAX ? 1U : mixed + 1U;
    }
    return left + right;
}

static uint64_t composite_revision(const UmiEditorAssistance *assistance)
{
    uint64_t revision;

    if (assistance == NULL) return 0U;
    revision = assistance->revision;
    revision = add_revision(revision,
                            umi_editor_hover_model_revision(assistance->hover));
    revision = add_revision(
        revision,
        umi_editor_signature_help_model_revision(assistance->signatures));
    revision = add_revision(
        revision,
        umi_editor_parameter_information_revision(assistance->parameters));
    return add_revision(
        revision,
        umi_editor_quick_documentation_revision(assistance->documentation));
}

static UmiStatus synchronize_active_parameter(UmiEditorAssistance *assistance)
{
    UmiEditorSignatureHelpItem signature;
    UmiEditorParameterInformationItem parameter;
    size_t ordinal;

    if (umi_editor_signature_help_model_active(assistance->signatures,
                                               &signature) != UMI_STATUS_OK) {
        (void)umi_editor_parameter_information_clear_active(
            assistance->parameters);
        return UMI_STATUS_NOT_FOUND;
    }
    if (signature.parameter_count == 0U) {
        return umi_editor_parameter_information_clear_active(
            assistance->parameters);
    }
    for (ordinal = 0U; ordinal < signature.parameter_count; ++ordinal) {
        if (umi_editor_parameter_information_for_signature_at(
                assistance->parameters, signature.id, ordinal,
                &parameter) == UMI_STATUS_OK) {
            return umi_editor_parameter_information_set_active(
                assistance->parameters, signature.id, parameter.ordinal);
        }
    }
    return umi_editor_parameter_information_clear_active(
        assistance->parameters);
}

UmiStatus umi_editor_assistance_create(
    UmiEditorAssistance **out_assistance)
{
    UmiEditorAssistance *assistance;
    UmiStatus status;

    if (out_assistance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_assistance = NULL;
    assistance = (UmiEditorAssistance *)calloc(1U, sizeof(*assistance));
    if (assistance == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_hover_model_create(&assistance->hover);
    if (status == UMI_STATUS_OK) {
        status = umi_editor_signature_help_model_create(&assistance->signatures);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_parameter_information_create(&assistance->parameters);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_quick_documentation_create(
            &assistance->documentation);
    }
    if (status != UMI_STATUS_OK) {
        umi_editor_assistance_destroy(assistance);
        return status;
    }
    assistance->hover_enabled = 1;
    assistance->signature_help_enabled = 1;
    assistance->parameter_information_enabled = 1;
    assistance->quick_documentation_enabled = 1;
    assistance->revision = 1U;
    *out_assistance = assistance;
    return UMI_STATUS_OK;
}

void umi_editor_assistance_destroy(UmiEditorAssistance *assistance)
{
    if (assistance == NULL) return;
    umi_editor_hover_model_destroy(assistance->hover);
    umi_editor_signature_help_model_destroy(assistance->signatures);
    umi_editor_parameter_information_destroy(assistance->parameters);
    umi_editor_quick_documentation_destroy(assistance->documentation);
    assistance->hover = NULL;
    assistance->signatures = NULL;
    assistance->parameters = NULL;
    assistance->documentation = NULL;
    free(assistance);
}

UmiStatus umi_editor_assistance_set_feature_enabled(
    UmiEditorAssistance *assistance,
    UmiEditorAssistanceFeature feature,
    int enabled)
{
    int value = enabled != 0;

    if (assistance == NULL || !valid_feature(feature)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (feature == UMI_EDITOR_ASSISTANCE_HOVER) {
        assistance->hover_enabled = value;
    } else if (feature == UMI_EDITOR_ASSISTANCE_SIGNATURE_HELP) {
        assistance->signature_help_enabled = value;
    } else if (feature == UMI_EDITOR_ASSISTANCE_PARAMETER_INFORMATION) {
        assistance->parameter_information_enabled = value;
    } else {
        assistance->quick_documentation_enabled = value;
    }
    assistance->revision = next_revision(assistance->revision);
    return UMI_STATUS_OK;
}

int umi_editor_assistance_feature_enabled(
    const UmiEditorAssistance *assistance,
    UmiEditorAssistanceFeature feature)
{
    if (assistance == NULL || !valid_feature(feature)) return 0;
    if (feature == UMI_EDITOR_ASSISTANCE_HOVER) {
        return assistance->hover_enabled;
    }
    if (feature == UMI_EDITOR_ASSISTANCE_SIGNATURE_HELP) {
        return assistance->signature_help_enabled;
    }
    if (feature == UMI_EDITOR_ASSISTANCE_PARAMETER_INFORMATION) {
        return assistance->parameter_information_enabled;
    }
    return assistance->quick_documentation_enabled;
}

UmiEditorHoverModel *umi_editor_assistance_hover(UmiEditorAssistance *assistance)
{
    return assistance != NULL ? assistance->hover : NULL;
}

UmiEditorSignatureHelpModel *umi_editor_assistance_signatures(
    UmiEditorAssistance *assistance)
{
    return assistance != NULL ? assistance->signatures : NULL;
}

UmiEditorParameterInformation *umi_editor_assistance_parameters(
    UmiEditorAssistance *assistance)
{
    return assistance != NULL ? assistance->parameters : NULL;
}

UmiEditorQuickDocumentation *umi_editor_assistance_documentation(
    UmiEditorAssistance *assistance)
{
    return assistance != NULL ? assistance->documentation : NULL;
}

UmiStatus umi_editor_assistance_activate_signature(
    UmiEditorAssistance *assistance,
    const char *signature_id)
{
    UmiStatus status;

    if (assistance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_signature_help_model_set_active(
        assistance->signatures, signature_id);
    if (status != UMI_STATUS_OK) return status;
    status = synchronize_active_parameter(assistance);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    assistance->revision = next_revision(assistance->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_assistance_select_next_signature(
    UmiEditorAssistance *assistance,
    int wrap)
{
    UmiStatus status;

    if (assistance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_signature_help_model_select_next(
        assistance->signatures, wrap);
    if (status != UMI_STATUS_OK) return status;
    status = synchronize_active_parameter(assistance);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    assistance->revision = next_revision(assistance->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_assistance_select_previous_signature(
    UmiEditorAssistance *assistance,
    int wrap)
{
    UmiStatus status;

    if (assistance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_signature_help_model_select_previous(
        assistance->signatures, wrap);
    if (status != UMI_STATUS_OK) return status;
    status = synchronize_active_parameter(assistance);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    assistance->revision = next_revision(assistance->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_assistance_activate_parameter(
    UmiEditorAssistance *assistance,
    size_t ordinal)
{
    UmiEditorSignatureHelpItem signature;
    UmiStatus status;

    if (assistance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_signature_help_model_active(assistance->signatures,
                                                    &signature);
    if (status != UMI_STATUS_OK) return status;
    if (ordinal >= signature.parameter_count) return UMI_STATUS_NOT_FOUND;
    status = umi_editor_parameter_information_set_active(
        assistance->parameters, signature.id, ordinal);
    if (status != UMI_STATUS_OK) return status;
    assistance->revision = next_revision(assistance->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_assistance_snapshot(
    const UmiEditorAssistance *assistance,
    UmiEditorAssistanceSnapshot *out_snapshot)
{
    UmiEditorHoverSnapshot hover_snapshot;
    UmiEditorSignatureHelpSnapshot signature_snapshot;
    UmiEditorParameterInformationSnapshot parameter_snapshot;
    UmiEditorQuickDocumentationSnapshot documentation_snapshot;

    if (assistance == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_editor_hover_model_snapshot(assistance->hover, &hover_snapshot) !=
            UMI_STATUS_OK ||
        umi_editor_signature_help_model_snapshot(
            assistance->signatures, &signature_snapshot) != UMI_STATUS_OK ||
        umi_editor_parameter_information_snapshot(
            assistance->parameters, &parameter_snapshot) != UMI_STATUS_OK ||
        umi_editor_quick_documentation_snapshot(
            assistance->documentation, &documentation_snapshot) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_ASSISTANCE_API_VERSION;
    out_snapshot->hover_section_count = hover_snapshot.section_count;
    out_snapshot->signature_count = signature_snapshot.signature_count;
    out_snapshot->parameter_count = parameter_snapshot.parameter_count;
    out_snapshot->documentation_section_count =
        documentation_snapshot.section_count;
    out_snapshot->revision = composite_revision(assistance);
    out_snapshot->hover_enabled = assistance->hover_enabled;
    out_snapshot->signature_help_enabled = assistance->signature_help_enabled;
    out_snapshot->parameter_information_enabled =
        assistance->parameter_information_enabled;
    out_snapshot->quick_documentation_enabled =
        assistance->quick_documentation_enabled;
    out_snapshot->hover_pinned = hover_snapshot.pinned;
    out_snapshot->signature_help_presented = signature_snapshot.presented;
    out_snapshot->documentation_pinned = documentation_snapshot.pinned;
    out_snapshot->has_active_signature =
        signature_snapshot.has_active_signature;
    out_snapshot->has_active_parameter =
        parameter_snapshot.has_active_parameter;
    return UMI_STATUS_OK;
}

uint64_t umi_editor_assistance_revision(
    const UmiEditorAssistance *assistance)
{
    return composite_revision(assistance);
}
