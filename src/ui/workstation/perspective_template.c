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

#include "umicom/ui/workstation/perspective_template.h"

/*
 * Initialise ws perspective template from caller-provided values so later operations
 * receive a known state.
 */
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

/*
 * Provide the ws perspective template set default layout operation used by this module and
 * its client applications.
 */
UmiStatus umi_ws_perspective_template_set_default_layout(UmiWsPerspectiveTemplate *perspective,
                                                         const char *layout_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (perspective == NULL || !umi_ws_id_valid(layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ws_copy_text(perspective->default_layout_id, sizeof(perspective->default_layout_id), layout_id);
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
