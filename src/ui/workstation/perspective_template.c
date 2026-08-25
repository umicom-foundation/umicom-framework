/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/perspective_template.c
 *
 * PURPOSE:
 *   Implement task-oriented workstation perspective metadata separately from concrete layout geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/perspective_template.h"

UmiStatus umi_ws_perspective_template_init(UmiWsPerspectiveTemplate *perspective,
                                           const char *perspective_id,
                                           const char *label,
                                           UmiWsApplicationDomain domain) {
    if (perspective == NULL || !umi_ws_id_valid(perspective_id) || label == NULL || label[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    *perspective = (UmiWsPerspectiveTemplate){0};
    if (umi_ws_copy_text(perspective->perspective_id, sizeof(perspective->perspective_id), perspective_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(perspective->label, sizeof(perspective->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    perspective->domain = domain;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_perspective_template_set_default_layout(UmiWsPerspectiveTemplate *perspective,
                                                         const char *layout_id) {
    if (perspective == NULL || !umi_ws_id_valid(layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ws_copy_text(perspective->default_layout_id, sizeof(perspective->default_layout_id), layout_id);
}

UmiStatus umi_ws_perspective_template_add_surface(UmiWsPerspectiveTemplate *perspective,
                                                  const char *surface_id) {
    if (perspective == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (perspective->preferred_surface_count >= UMI_WS_MAX_SURFACES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(perspective->preferred_surface_ids[perspective->preferred_surface_count],
                         sizeof(perspective->preferred_surface_ids[perspective->preferred_surface_count]),
                         surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++perspective->preferred_surface_count;
    return UMI_STATUS_OK;
}
