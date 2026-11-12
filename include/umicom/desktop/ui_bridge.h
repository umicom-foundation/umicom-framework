/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/ui_bridge.h
 *
 * PURPOSE:
 *   Publish the public ui bridge contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_UI_BRIDGE_H
#define UMICOM_DESKTOP_UI_BRIDGE_H

#include "umicom/desktop/context_links.h"
#include "umicom/desktop/layout_catalogue.h"
#include "umicom/desktop/monitor_topology.h"
#include "umicom/ui/workspace_customisation.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the desktop seed workspace customisation operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_seed_workspace_customisation(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiDesktopLayoutCatalogue *catalogue,
    const UmiDesktopMonitorTopology *topology,
    const char *preset_namespace);
/**
 * Provide the desktop seed workspace groups operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_seed_workspace_groups(
    UmiUiWindowGroupStore *groups,
    const UmiDesktopContextLinks *context_links);
/**
 * Provide the desktop seed window catalogue operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_seed_window_catalogue(
    UmiUiWindowCatalogue *catalogue);


#ifdef __cplusplus
}
#endif
#endif
