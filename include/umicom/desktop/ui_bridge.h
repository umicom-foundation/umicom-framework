/*-----------------------------------------------------------------------------
 * Umicom Framework | Desktop to semantic UI bridge
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_UI_BRIDGE_H
#define UMICOM_DESKTOP_UI_BRIDGE_H

#include "umicom/desktop/context_links.h"
#include "umicom/desktop/layout_catalogue.h"
#include "umicom/desktop/monitor_topology.h"
#include "umicom/ui/workspace_customisation.h"

UmiStatus umi_desktop_seed_workspace_customisation(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiDesktopLayoutCatalogue *catalogue,
    const UmiDesktopMonitorTopology *topology,
    const char *preset_namespace);
UmiStatus umi_desktop_seed_workspace_groups(
    UmiUiWindowGroupStore *groups,
    const UmiDesktopContextLinks *context_links);
UmiStatus umi_desktop_seed_window_catalogue(
    UmiUiWindowCatalogue *catalogue);

#endif
