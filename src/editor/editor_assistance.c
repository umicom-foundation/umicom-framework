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

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the valid feature operation used by this module and its client applications. */
static int valid_feature(UmiEditorAssistanceFeature feature)
{
    return feature >= UMI_EDITOR_ASSISTANCE_HOVER &&
           feature <= UMI_EDITOR_ASSISTANCE_QUICK_DOCUMENTATION;
}

/* Provide the add revision operation used by this module and its client applications. */
static uint64_t add_revision(uint64_t left, uint64_t right)
{
    uint64_t mixed;

    /* Apply this branch only when its contract condition is satisfied. */
    if (UINT64_MAX - left < right) {
        mixed = left ^ right;
        return mixed == UINT64_MAX ? 1U : mixed + 1U;
    }
    return left + right;
}

/*
 * Provide the composite revision operation used by this module and its client
 * applications.
 */
static uint64_t composite_revision(const UmiEditorAssistance *assistance)
{
    uint64_t revision;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the synchronize active parameter operation used by this module and its client
 * applications.
 */
static UmiStatus synchronize_active_parameter(UmiEditorAssistance *assistance)
{
    UmiEditorSignatureHelpItem signature;
    UmiEditorParameterInformationItem parameter;
    size_t ordinal;

    /* Apply this operation only while the related capability or state is available. */
    if (umi_editor_signature_help_model_active(assistance->signatures,
                                               &signature) != UMI_STATUS_OK) {
        (void)umi_editor_parameter_information_clear_active(
            assistance->parameters);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (signature.parameter_count == 0U) {
        return umi_editor_parameter_information_clear_active(
            assistance->parameters);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (ordinal = 0U; ordinal < signature.parameter_count; ++ordinal) {
        /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Initialise editor assistance from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_assistance_create(
    UmiEditorAssistance **out_assistance)
{
    UmiEditorAssistance *assistance;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_assistance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_assistance = NULL;
    assistance = (UmiEditorAssistance *)calloc(1U, sizeof(*assistance));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (assistance == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_hover_model_create(&assistance->hover);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_signature_help_model_create(&assistance->signatures);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_parameter_information_create(&assistance->parameters);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_quick_documentation_create(
            &assistance->documentation);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Release or reset state held by editor assistance so the same storage can be reused
 * safely.
 */
void umi_editor_assistance_destroy(UmiEditorAssistance *assistance)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the editor assistance set feature enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_assistance_set_feature_enabled(
    UmiEditorAssistance *assistance,
    UmiEditorAssistanceFeature feature,
    int enabled)
{
    int value = enabled != 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (assistance == NULL || !valid_feature(feature)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature == UMI_EDITOR_ASSISTANCE_HOVER) {
        assistance->hover_enabled = value;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (feature == UMI_EDITOR_ASSISTANCE_SIGNATURE_HELP) {
        assistance->signature_help_enabled = value;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (feature == UMI_EDITOR_ASSISTANCE_PARAMETER_INFORMATION) {
        assistance->parameter_information_enabled = value;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        assistance->quick_documentation_enabled = value;
    }
    assistance->revision = next_revision(assistance->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor assistance feature enabled operation used by this module and its
 * client applications.
 */
int umi_editor_assistance_feature_enabled(
    const UmiEditorAssistance *assistance,
    UmiEditorAssistanceFeature feature)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (assistance == NULL || !valid_feature(feature)) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature == UMI_EDITOR_ASSISTANCE_HOVER) {
        return assistance->hover_enabled;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature == UMI_EDITOR_ASSISTANCE_SIGNATURE_HELP) {
        return assistance->signature_help_enabled;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature == UMI_EDITOR_ASSISTANCE_PARAMETER_INFORMATION) {
        return assistance->parameter_information_enabled;
    }
    return assistance->quick_documentation_enabled;
}

/*
 * Provide the editor assistance hover operation used by this module and its client
 * applications.
 */
UmiEditorHoverModel *umi_editor_assistance_hover(UmiEditorAssistance *assistance)
{
    return assistance != NULL ? assistance->hover : NULL;
}

/*
 * Provide the editor assistance signatures operation used by this module and its client
 * applications.
 */
UmiEditorSignatureHelpModel *umi_editor_assistance_signatures(
    UmiEditorAssistance *assistance)
{
    return assistance != NULL ? assistance->signatures : NULL;
}

/*
 * Provide the editor assistance parameters operation used by this module and its client
 * applications.
 */
UmiEditorParameterInformation *umi_editor_assistance_parameters(
    UmiEditorAssistance *assistance)
{
    return assistance != NULL ? assistance->parameters : NULL;
}

/*
 * Provide the editor assistance documentation operation used by this module and its client
 * applications.
 */
UmiEditorQuickDocumentation *umi_editor_assistance_documentation(
    UmiEditorAssistance *assistance)
{
    return assistance != NULL ? assistance->documentation : NULL;
}

/*
 * Provide the editor assistance activate signature operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_assistance_activate_signature(
    UmiEditorAssistance *assistance,
    const char *signature_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (assistance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_signature_help_model_set_active(
        assistance->signatures, signature_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = synchronize_active_parameter(assistance);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    assistance->revision = next_revision(assistance->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor assistance select next signature operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_assistance_select_next_signature(
    UmiEditorAssistance *assistance,
    int wrap)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (assistance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_signature_help_model_select_next(
        assistance->signatures, wrap);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = synchronize_active_parameter(assistance);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    assistance->revision = next_revision(assistance->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor assistance select previous signature operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_assistance_select_previous_signature(
    UmiEditorAssistance *assistance,
    int wrap)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (assistance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_signature_help_model_select_previous(
        assistance->signatures, wrap);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = synchronize_active_parameter(assistance);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    assistance->revision = next_revision(assistance->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor assistance activate parameter operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_assistance_activate_parameter(
    UmiEditorAssistance *assistance,
    size_t ordinal)
{
    UmiEditorSignatureHelpItem signature;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (assistance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_signature_help_model_active(assistance->signatures,
                                                    &signature);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (ordinal >= signature.parameter_count) return UMI_STATUS_NOT_FOUND;
    status = umi_editor_parameter_information_set_active(
        assistance->parameters, signature.id, ordinal);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    assistance->revision = next_revision(assistance->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor assistance snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_assistance_snapshot(
    const UmiEditorAssistance *assistance,
    UmiEditorAssistanceSnapshot *out_snapshot)
{
    UmiEditorHoverSnapshot hover_snapshot;
    UmiEditorSignatureHelpSnapshot signature_snapshot;
    UmiEditorParameterInformationSnapshot parameter_snapshot;
    UmiEditorQuickDocumentationSnapshot documentation_snapshot;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (assistance == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the editor assistance revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_assistance_revision(
    const UmiEditorAssistance *assistance)
{
    return composite_revision(assistance);
}
