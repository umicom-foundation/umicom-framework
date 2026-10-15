/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/perspective_template.c
 *
 * PURPOSE:
 *   Implement task-oriented workstation perspective metadata separately from concrete layout geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/text.h"
#include "umicom/ui/workstation/perspective_template.h"
#include "../../base/value_construction_internal.h"

/*
 * Initialise ws perspective template from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_ws_perspective_template_init(UmiWsPerspectiveTemplate *perspective,
                                           const char *perspective_id,
                                           const char *label,
                                           UmiWsApplicationDomain domain) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (perspective == NULL || !umi_ws_id_valid(perspective_id) || label == NULL || label[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    *perspective = (UmiWsPerspectiveTemplate){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(perspective->perspective_id, sizeof(perspective->perspective_id), perspective_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(perspective->label, sizeof(perspective->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    perspective->domain = domain;
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_ws_perspective_template_init_build_value(UmiWsPerspectiveTemplate *perspective,
                                           const char *perspective_id,
                                           const char *label,
                                           UmiWsApplicationDomain domain) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (perspective == NULL || !umi_ws_id_valid(perspective_id) || label == NULL || label[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    *perspective = (UmiWsPerspectiveTemplate){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(perspective->perspective_id, sizeof(perspective->perspective_id), perspective_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(perspective->label, sizeof(perspective->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    perspective->domain = domain;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_ws_perspective_template_init, UmiWsPerspectiveTemplate, perspective,
    (UmiWsPerspectiveTemplate *perspective, const char *perspective_id, const char *label, UmiWsApplicationDomain domain),
    umi_ws_perspective_template_init_build_value(candidate, perspective_id, label, domain))


/*
 * Provide the ws perspective template set default layout operation used by this module and
 * its client applications.
 */
/* The presentation copy could erase a rejected field or overlap its input.
 * Retain it for review; the replacement validates before publishing any text. */
#if 0
UmiStatus umi_ws_perspective_template_set_default_layout(UmiWsPerspectiveTemplate *perspective,
                                                         const char *layout_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (perspective == NULL || !umi_ws_id_valid(layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ws_copy_text(perspective->default_layout_id, sizeof(perspective->default_layout_id), layout_id);
}
#endif

UmiStatus umi_ws_perspective_template_set_default_layout(UmiWsPerspectiveTemplate *perspective,
                                                         const char *layout_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (perspective == NULL || !umi_ws_id_valid(layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_replace(perspective->default_layout_id, sizeof(perspective->default_layout_id), layout_id);
}

/*
 * Provide the ws perspective template add surface operation used by this module and its
 * client applications.
 */
UmiStatus umi_ws_perspective_template_add_surface(UmiWsPerspectiveTemplate *perspective,
                                                  const char *surface_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (perspective == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (perspective->preferred_surface_count >= UMI_WS_MAX_SURFACES) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_ws_copy_text(perspective->preferred_surface_ids[perspective->preferred_surface_count],
                         sizeof(perspective->preferred_surface_ids[perspective->preferred_surface_count]),
                         surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++perspective->preferred_surface_count;
    return UMI_STATUS_OK;
}
