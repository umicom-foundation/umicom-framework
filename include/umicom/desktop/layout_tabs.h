/*-----------------------------------------------------------------------------
 * Umicom Framework | TWS-inspired bottom desktop layout tabs
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_LAYOUT_TABS_H
#define UMICOM_DESKTOP_LAYOUT_TABS_H

#include "umicom/base/status.h"
#include "umicom/desktop/types.h"

typedef struct UmiDesktopLayoutTabs {
    UmiDesktopLayoutTab tabs[UMI_DESKTOP_MAX_TABS];
    size_t count;
    UmiDesktopLayoutTabPlacement placement;
    uint64_t revision;
} UmiDesktopLayoutTabs;

void umi_desktop_layout_tabs_init(UmiDesktopLayoutTabs *tabs);
UmiStatus umi_desktop_layout_tabs_add(
    UmiDesktopLayoutTabs *tabs,
    const UmiDesktopLayoutTab *tab);
UmiStatus umi_desktop_layout_tabs_remove(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id);
UmiStatus umi_desktop_layout_tabs_activate(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id);
UmiStatus umi_desktop_layout_tabs_move(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    size_t new_index);
UmiStatus umi_desktop_layout_tabs_rename(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    const char *label);
UmiStatus umi_desktop_layout_tabs_set_dirty(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    bool dirty);
const UmiDesktopLayoutTab *umi_desktop_layout_tabs_find(
    const UmiDesktopLayoutTabs *tabs,
    const char *tab_id);
const UmiDesktopLayoutTab *umi_desktop_layout_tabs_active(
    const UmiDesktopLayoutTabs *tabs);

#endif
