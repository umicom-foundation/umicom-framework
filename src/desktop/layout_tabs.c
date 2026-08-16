/*-----------------------------------------------------------------------------
 * Umicom Framework | TWS-inspired bottom desktop layout tabs
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/layout_tabs.h"

#include <stdio.h>
#include <string.h>

static size_t find_tab_index(
    const UmiDesktopLayoutTabs *tabs,
    const char *tab_id)
{
    size_t index;
    if (tabs == NULL || tab_id == NULL) return SIZE_MAX;
    for (index = 0U; index < tabs->count; ++index) {
        if (strcmp(tabs->tabs[index].tab_id, tab_id) == 0) return index;
    }
    return SIZE_MAX;
}

static void refresh_order(UmiDesktopLayoutTabs *tabs)
{
    size_t index;
    for (index = 0U; index < tabs->count; ++index)
        tabs->tabs[index].order = (uint32_t)index;
}

void umi_desktop_layout_tabs_init(UmiDesktopLayoutTabs *tabs)
{
    if (tabs == NULL) return;
    (void)memset(tabs, 0, sizeof(*tabs));
    tabs->placement = UMI_DESKTOP_LAYOUT_TABS_BOTTOM;
}

UmiStatus umi_desktop_layout_tabs_add(
    UmiDesktopLayoutTabs *tabs,
    const UmiDesktopLayoutTab *tab)
{
    if (tabs == NULL || tab == NULL || tab->tab_id[0] == '\0' ||
        tab->layout_id[0] == '\0' || tab->label[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (find_tab_index(tabs, tab->tab_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    if (tabs->count >= UMI_DESKTOP_MAX_TABS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    tabs->tabs[tabs->count] = *tab;
    tabs->tabs[tabs->count].order = (uint32_t)tabs->count;
    if (tabs->count == 0U) tabs->tabs[0].active = true;
    tabs->count += 1U;
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_tabs_remove(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id)
{
    size_t index;
    bool active;
    if (tabs == NULL || tab_id == NULL || tab_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_tab_index(tabs, tab_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (!tabs->tabs[index].closable || tabs->tabs[index].pinned)
        return UMI_STATUS_PERMISSION_DENIED;
    active = tabs->tabs[index].active;
    if (index + 1U < tabs->count) {
        (void)memmove(&tabs->tabs[index], &tabs->tabs[index + 1U],
                      (tabs->count - index - 1U) * sizeof(tabs->tabs[0]));
    }
    tabs->count -= 1U;
    (void)memset(&tabs->tabs[tabs->count], 0, sizeof(tabs->tabs[0]));
    refresh_order(tabs);
    if (active && tabs->count > 0U)
        tabs->tabs[index < tabs->count ? index : tabs->count - 1U].active = true;
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_tabs_activate(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id)
{
    size_t index;
    size_t selected;
    if (tabs == NULL || tab_id == NULL || tab_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    selected = find_tab_index(tabs, tab_id);
    if (selected == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < tabs->count; ++index)
        tabs->tabs[index].active = index == selected;
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_tabs_move(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    size_t new_index)
{
    size_t old_index;
    UmiDesktopLayoutTab moved;
    if (tabs == NULL || tab_id == NULL || new_index >= tabs->count)
        return UMI_STATUS_INVALID_ARGUMENT;
    old_index = find_tab_index(tabs, tab_id);
    if (old_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (old_index == new_index) return UMI_STATUS_OK;
    moved = tabs->tabs[old_index];
    if (old_index < new_index) {
        (void)memmove(&tabs->tabs[old_index], &tabs->tabs[old_index + 1U],
                      (new_index - old_index) * sizeof(tabs->tabs[0]));
    } else {
        (void)memmove(&tabs->tabs[new_index + 1U], &tabs->tabs[new_index],
                      (old_index - new_index) * sizeof(tabs->tabs[0]));
    }
    tabs->tabs[new_index] = moved;
    refresh_order(tabs);
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_tabs_rename(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    const char *label)
{
    size_t index;
    int length;
    if (tabs == NULL || label == NULL || label[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_tab_index(tabs, tab_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    length = snprintf(tabs->tabs[index].label,
                      sizeof(tabs->tabs[index].label), "%s", label);
    if (length < 0 || (size_t)length >= sizeof(tabs->tabs[index].label))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_tabs_set_dirty(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    bool dirty)
{
    const size_t index = find_tab_index(tabs, tab_id);
    if (tabs == NULL || tab_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    tabs->tabs[index].dirty = dirty;
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiDesktopLayoutTab *umi_desktop_layout_tabs_find(
    const UmiDesktopLayoutTabs *tabs,
    const char *tab_id)
{
    const size_t index = find_tab_index(tabs, tab_id);
    return index != SIZE_MAX ? &tabs->tabs[index] : NULL;
}

const UmiDesktopLayoutTab *umi_desktop_layout_tabs_active(
    const UmiDesktopLayoutTabs *tabs)
{
    size_t index;
    if (tabs == NULL) return NULL;
    for (index = 0U; index < tabs->count; ++index) {
        if (tabs->tabs[index].active) return &tabs->tabs[index];
    }
    return NULL;
}
