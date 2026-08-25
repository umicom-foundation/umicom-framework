/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/workstation.h
 *
 * PURPOSE:
 *   Define the aggregate reusable workstation state that composes surfaces, layouts, perspectives, workspaces, density, themes and accessibility.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_WORKSTATION_H
#define UMICOM_UI_WORKSTATION_WORKSTATION_H

#include "umicom/ui/workstation/accessibility_profile.h"
#include "umicom/ui/workstation/application_surface_set.h"
#include "umicom/ui/workstation/density_profile.h"
#include "umicom/ui/workstation/layout_catalogue.h"
#include "umicom/ui/workstation/layout_lock.h"
#include "umicom/ui/workstation/perspective_catalogue.h"
#include "umicom/ui/workstation/status_strip.h"
#include "umicom/ui/workstation/surface_catalogue.h"
#include "umicom/ui/workstation/toolbar_model.h"
#include "umicom/ui/workstation/workspace_strip.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsWorkstation {
    char workstation_id[UMI_UI_ID_CAPACITY];
    UmiWsSurfaceCatalogue surfaces;
    UmiWsLayoutCatalogue layouts;
    UmiWsPerspectiveCatalogue perspectives;
    UmiWsWorkspaceStrip workspaces;
    UmiWsToolbarModel toolbar;
    UmiWsStatusStrip status;
    UmiWsDensityProfile density;
    UmiWsAccessibilityProfile accessibility;
    UmiWsLayoutLock layout_lock;
    char active_layout_id[UMI_UI_ID_CAPACITY];
    char active_perspective_id[UMI_UI_ID_CAPACITY];
    uint64_t revision;
} UmiWsWorkstation;

UmiStatus umi_ws_workstation_init(UmiWsWorkstation *workstation,
                                  const char *workstation_id,
                                  UmiWsDensity density);
UmiStatus umi_ws_workstation_activate_layout(UmiWsWorkstation *workstation, const char *layout_id);
UmiStatus umi_ws_workstation_activate_perspective(UmiWsWorkstation *workstation, const char *perspective_id);
bool umi_ws_workstation_ready(const UmiWsWorkstation *workstation);
uint64_t umi_ws_workstation_fingerprint(const UmiWsWorkstation *workstation);

#ifdef __cplusplus
}
#endif

#endif
