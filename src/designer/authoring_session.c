/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/authoring_session.c
 *
 * PURPOSE:
 *   Implement an owned visual-authoring session combining signal/action bindings, property schemas, alignment operations, clipboard items and templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation owns only its documented child models. Product-specific
 * business data and toolkit objects remain outside this reusable Framework layer.
 */
#include "umicom/designer/authoring_session.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesignerAuthoringSession {
    UmiDesignerSignalBindingRegistry *signals;
    UmiDesignerActionBindingRegistry *actions;
    UmiDesignerPropertySchemaRegistry *properties;
    UmiDesignerAlignmentRegistry *alignments;
    UmiDesignerClipboardItemRegistry *clipboard;
    UmiDesignerTemplatePaletteRegistry *templates;
    uint64_t revision;
};

/*
 * Initialise designer authoring session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_authoring_session_create(UmiDesignerAuthoringSession **out_service)
{
    UmiDesignerAuthoringSession *service;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiDesignerAuthoringSession *)calloc(1U, sizeof(*service));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->revision = 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_signal_binding_registry_create(&service->signals);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_action_binding_registry_create(&service->actions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_property_schema_registry_create(&service->properties);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_alignment_registry_create(&service->alignments);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_clipboard_registry_create(&service->clipboard);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_template_palette_registry_create(&service->templates);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_designer_authoring_session_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by designer authoring session so the same storage can be
 * reused safely.
 */
void umi_designer_authoring_session_destroy(UmiDesignerAuthoringSession *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    umi_designer_template_palette_registry_destroy(service->templates);
    umi_designer_clipboard_registry_destroy(service->clipboard);
    umi_designer_alignment_registry_destroy(service->alignments);
    umi_designer_property_schema_registry_destroy(service->properties);
    umi_designer_action_binding_registry_destroy(service->actions);
    umi_designer_signal_binding_registry_destroy(service->signals);
    free(service);
}

/*
 * Provide the designer authoring session snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_authoring_session_snapshot(const UmiDesignerAuthoringSession *service, UmiDesignerAuthoringSessionSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    out_snapshot->signals_count = umi_designer_signal_binding_registry_count(service->signals);
    out_snapshot->actions_count = umi_designer_action_binding_registry_count(service->actions);
    out_snapshot->properties_count = umi_designer_property_schema_registry_count(service->properties);
    out_snapshot->alignments_count = umi_designer_alignment_registry_count(service->alignments);
    out_snapshot->clipboard_count = umi_designer_clipboard_registry_count(service->clipboard);
    out_snapshot->templates_count = umi_designer_template_palette_registry_count(service->templates);
    out_snapshot->revision = service->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the designer authoring session signals operation used by this module and its
 * client applications.
 */
UmiDesignerSignalBindingRegistry *umi_designer_authoring_session_signals(UmiDesignerAuthoringSession *service)
{
    return service != NULL ? service->signals : NULL;
}

/*
 * Provide the designer authoring session actions operation used by this module and its
 * client applications.
 */
UmiDesignerActionBindingRegistry *umi_designer_authoring_session_actions(UmiDesignerAuthoringSession *service)
{
    return service != NULL ? service->actions : NULL;
}

/*
 * Provide the designer authoring session properties operation used by this module and its
 * client applications.
 */
UmiDesignerPropertySchemaRegistry *umi_designer_authoring_session_properties(UmiDesignerAuthoringSession *service)
{
    return service != NULL ? service->properties : NULL;
}

/*
 * Provide the designer authoring session alignments operation used by this module and its
 * client applications.
 */
UmiDesignerAlignmentRegistry *umi_designer_authoring_session_alignments(UmiDesignerAuthoringSession *service)
{
    return service != NULL ? service->alignments : NULL;
}

/*
 * Provide the designer authoring session clipboard operation used by this module and its
 * client applications.
 */
UmiDesignerClipboardItemRegistry *umi_designer_authoring_session_clipboard(UmiDesignerAuthoringSession *service)
{
    return service != NULL ? service->clipboard : NULL;
}

/*
 * Provide the designer authoring session templates operation used by this module and its
 * client applications.
 */
UmiDesignerTemplatePaletteRegistry *umi_designer_authoring_session_templates(UmiDesignerAuthoringSession *service)
{
    return service != NULL ? service->templates : NULL;
}
