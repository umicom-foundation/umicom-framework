/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/layout_tabs.h
 *
 * PURPOSE:
 *   Publish the public layout tabs contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_LAYOUT_TABS_H
#define UMICOM_DESKTOP_LAYOUT_TABS_H

#include "umicom/base/status.h"
#include "umicom/desktop/types.h"

/**
 * Represent the desktop layout tabs data shared with callers of this public contract.
 */
typedef struct UmiDesktopLayoutTabs {
    UmiDesktopLayoutTab tabs[UMI_DESKTOP_MAX_TABS];
    size_t count;
    UmiDesktopLayoutTabPlacement placement;
    uint64_t revision;
} UmiDesktopLayoutTabs;

/**
 * Initialise desktop layout tabs from caller-provided values so later operations receive a
 * known state.
 */
void umi_desktop_layout_tabs_init(UmiDesktopLayoutTabs *tabs);
/**
 * Add desktop layout tabs only after its inputs and available capacity have been checked.
 */
UmiStatus umi_desktop_layout_tabs_add(
    UmiDesktopLayoutTabs *tabs,
    const UmiDesktopLayoutTab *tab);
/**
 * Remove desktop layout tabs while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_desktop_layout_tabs_remove(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id);
/**
 * Provide the desktop layout tabs activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_tabs_activate(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id);
/**
 * Provide the desktop layout tabs move operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_tabs_move(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    size_t new_index);
/**
 * Provide the desktop layout tabs rename operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_tabs_rename(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    const char *label);
/**
 * Provide the desktop layout tabs set dirty operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_tabs_set_dirty(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    bool dirty);
/**
 * Find desktop layout tabs while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDesktopLayoutTab *umi_desktop_layout_tabs_find(
    const UmiDesktopLayoutTabs *tabs,
    const char *tab_id);
/**
 * Provide the desktop layout tabs active operation used by this module and its client
 * applications.
 */
const UmiDesktopLayoutTab *umi_desktop_layout_tabs_active(
    const UmiDesktopLayoutTabs *tabs);

#endif
