/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/workstation.c
 *
 * PURPOSE:
 *   Implement the aggregate reusable workstation state that composes surfaces, layouts, perspectives, workspaces, density, themes and accessibility.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/workstation.h"

UmiStatus umi_ws_workstation_init(UmiWsWorkstation *workstation,
                                  const char *workstation_id,
                                  UmiWsDensity density) {
    if (workstation == NULL || !umi_ws_id_valid(workstation_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *workstation = (UmiWsWorkstation){0};
    if (umi_ws_copy_text(workstation->workstation_id, sizeof(workstation->workstation_id), workstation_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_ws_surface_catalogue_init(&workstation->surfaces);
    umi_ws_layout_catalogue_init(&workstation->layouts);
    umi_ws_perspective_catalogue_init(&workstation->perspectives);
    umi_ws_workspace_strip_init(&workstation->workspaces);
    umi_ws_toolbar_model_init(&workstation->toolbar);
    umi_ws_status_strip_init(&workstation->status);
    workstation->density = umi_ws_density_profile_make(density);
    workstation->accessibility = umi_ws_accessibility_profile_default(density);
    workstation->layout_lock = umi_ws_layout_lock_default();
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_workstation_activate_layout(UmiWsWorkstation *workstation, const char *layout_id) {
    if (workstation == NULL || !umi_ws_id_valid(layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_ws_layout_catalogue_find(&workstation->layouts, layout_id) == NULL) return UMI_STATUS_NOT_FOUND;
    if (umi_ws_copy_text(workstation->active_layout_id, sizeof(workstation->active_layout_id), layout_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++workstation->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_workstation_activate_perspective(UmiWsWorkstation *workstation, const char *perspective_id) {
    if (workstation == NULL || !umi_ws_id_valid(perspective_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_ws_perspective_catalogue_find(&workstation->perspectives, perspective_id) == NULL) return UMI_STATUS_NOT_FOUND;
    if (umi_ws_copy_text(workstation->active_perspective_id, sizeof(workstation->active_perspective_id), perspective_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++workstation->revision;
    return UMI_STATUS_OK;
}

bool umi_ws_workstation_ready(const UmiWsWorkstation *workstation) {
    if (workstation == NULL) return false;
    return workstation->surfaces.count > 0U &&
           workstation->layouts.count > 0U &&
           workstation->perspectives.count > 0U &&
           workstation->active_layout_id[0] != '\0' &&
           workstation->active_perspective_id[0] != '\0';
}

uint64_t umi_ws_workstation_fingerprint(const UmiWsWorkstation *workstation) {
    uint64_t hash;
    if (workstation == NULL) return 0U;
    hash = umi_ws_hash_text(workstation->workstation_id);
    hash ^= umi_ws_hash_text(workstation->active_layout_id);
    hash ^= umi_ws_hash_text(workstation->active_perspective_id);
    hash ^= (uint64_t)workstation->surfaces.count << 8U;
    hash ^= (uint64_t)workstation->layouts.count << 16U;
    hash ^= (uint64_t)workstation->perspectives.count << 24U;
    return hash;
}
