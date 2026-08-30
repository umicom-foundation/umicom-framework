/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_layout_tabs.c
 *
 * PURPOSE:
 *   Implement the test layout tabs behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Desktop layout tabs | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/desktop/layout_tabs.h"

static UmiDesktopLayoutTab tab(const char *id, const char *label)
{
    UmiDesktopLayoutTab value = {0};
    (void)snprintf(value.tab_id, sizeof(value.tab_id), "%s", id);
    (void)snprintf(value.layout_id, sizeof(value.layout_id), "%s", id);
    (void)snprintf(value.label, sizeof(value.label), "%s", label);
    value.closable = true;
    return value;
}

int main(void)
{
    UmiDesktopLayoutTabs tabs;
    UmiDesktopLayoutTab develop = tab("develop", "Develop");
    UmiDesktopLayoutTab trading = tab("trading", "Trading");
    umi_desktop_layout_tabs_init(&tabs);
    assert(tabs.placement == UMI_DESKTOP_LAYOUT_TABS_BOTTOM);
    assert(umi_desktop_layout_tabs_add(&tabs, &develop) == UMI_STATUS_OK);
    assert(umi_desktop_layout_tabs_add(&tabs, &trading) == UMI_STATUS_OK);
    assert(umi_desktop_layout_tabs_activate(&tabs, "trading") == UMI_STATUS_OK);
    assert(strcmp(umi_desktop_layout_tabs_active(&tabs)->tab_id, "trading") == 0);
    assert(umi_desktop_layout_tabs_move(&tabs, "trading", 0U) == UMI_STATUS_OK);
    assert(strcmp(tabs.tabs[0].tab_id, "trading") == 0);
    assert(umi_desktop_layout_tabs_set_dirty(&tabs, "trading", true) ==
           UMI_STATUS_OK);
    assert(umi_desktop_layout_tabs_find(&tabs, "trading")->dirty);
    assert(umi_desktop_layout_tabs_rename(&tabs, "trading", "My Trading") ==
           UMI_STATUS_OK);
    assert(strcmp(tabs.tabs[0].label, "My Trading") == 0);
    assert(umi_desktop_layout_tabs_remove(&tabs, "develop") == UMI_STATUS_OK);
    assert(tabs.count == 1U);
    return 0;
}
