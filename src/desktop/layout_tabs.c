/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/layout_tabs.c
 *
 * PURPOSE:
 *   Implement the layout tabs behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework | Professional bottom desktop layout tabs
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/layout_tabs.h"

#include <stdio.h>
#include <string.h>

/* Provide the find tab index operation used by this module and its client applications. */
static size_t find_tab_index(
    const UmiDesktopLayoutTabs *tabs,
    const char *tab_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tabs == NULL || tab_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < tabs->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(tabs->tabs[index].tab_id, tab_id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the refresh order operation used by this module and its client applications. */
static void refresh_order(UmiDesktopLayoutTabs *tabs)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < tabs->count; ++index)
        tabs->tabs[index].order = (uint32_t)index;
}

/*
 * Initialise desktop layout tabs from caller-provided values so later operations receive a
 * known state.
 */
void umi_desktop_layout_tabs_init(UmiDesktopLayoutTabs *tabs)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tabs == NULL) return;
    (void)memset(tabs, 0, sizeof(*tabs));
    tabs->placement = UMI_DESKTOP_LAYOUT_TABS_BOTTOM;
}

/* Add desktop layout tabs only after its inputs and available capacity have been checked. */
UmiStatus umi_desktop_layout_tabs_add(
    UmiDesktopLayoutTabs *tabs,
    const UmiDesktopLayoutTab *tab)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tabs == NULL || tab == NULL || tab->tab_id[0] == '\0' ||
        tab->layout_id[0] == '\0' || tab->label[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_tab_index(tabs, tab->tab_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (tabs->count >= UMI_DESKTOP_MAX_TABS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    tabs->tabs[tabs->count] = *tab;
    tabs->tabs[tabs->count].order = (uint32_t)tabs->count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (tabs->count == 0U) tabs->tabs[0].active = true;
    tabs->count += 1U;
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove desktop layout tabs while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_desktop_layout_tabs_remove(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id)
{
    size_t index;
    bool active;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tabs == NULL || tab_id == NULL || tab_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_tab_index(tabs, tab_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!tabs->tabs[index].closable || tabs->tabs[index].pinned)
        return UMI_STATUS_PERMISSION_DENIED;
    active = tabs->tabs[index].active;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < tabs->count) {
        (void)memmove(&tabs->tabs[index], &tabs->tabs[index + 1U],
                      (tabs->count - index - 1U) * sizeof(tabs->tabs[0]));
    }
    tabs->count -= 1U;
    (void)memset(&tabs->tabs[tabs->count], 0, sizeof(tabs->tabs[0]));
    refresh_order(tabs);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (active && tabs->count > 0U)
        tabs->tabs[index < tabs->count ? index : tabs->count - 1U].active = true;
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop layout tabs activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_tabs_activate(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id)
{
    size_t index;
    size_t selected;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tabs == NULL || tab_id == NULL || tab_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    selected = find_tab_index(tabs, tab_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (selected == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < tabs->count; ++index)
        tabs->tabs[index].active = index == selected;
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop layout tabs move operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_tabs_move(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    size_t new_index)
{
    size_t old_index;
    UmiDesktopLayoutTab moved;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tabs == NULL || tab_id == NULL || new_index >= tabs->count)
        return UMI_STATUS_INVALID_ARGUMENT;
    old_index = find_tab_index(tabs, tab_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (old_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (old_index == new_index) return UMI_STATUS_OK;
    moved = tabs->tabs[old_index];
    /* Apply this branch only when its contract condition is satisfied. */
    if (old_index < new_index) {
        (void)memmove(&tabs->tabs[old_index], &tabs->tabs[old_index + 1U],
                      (new_index - old_index) * sizeof(tabs->tabs[0]));
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)memmove(&tabs->tabs[new_index + 1U], &tabs->tabs[new_index],
                      (old_index - new_index) * sizeof(tabs->tabs[0]));
    }
    tabs->tabs[new_index] = moved;
    refresh_order(tabs);
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop layout tabs rename operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_tabs_rename(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    const char *label)
{
    size_t index;
    int length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tabs == NULL || label == NULL || label[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_tab_index(tabs, tab_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    length = snprintf(tabs->tabs[index].label,
                      sizeof(tabs->tabs[index].label), "%s", label);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0 || (size_t)length >= sizeof(tabs->tabs[index].label))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop layout tabs set dirty operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_tabs_set_dirty(
    UmiDesktopLayoutTabs *tabs,
    const char *tab_id,
    bool dirty)
{
    const size_t index = find_tab_index(tabs, tab_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tabs == NULL || tab_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    tabs->tabs[index].dirty = dirty;
    tabs->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find desktop layout tabs while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDesktopLayoutTab *umi_desktop_layout_tabs_find(
    const UmiDesktopLayoutTabs *tabs,
    const char *tab_id)
{
    const size_t index = find_tab_index(tabs, tab_id);
    return index != SIZE_MAX ? &tabs->tabs[index] : NULL;
}

/*
 * Provide the desktop layout tabs active operation used by this module and its client
 * applications.
 */
const UmiDesktopLayoutTab *umi_desktop_layout_tabs_active(
    const UmiDesktopLayoutTabs *tabs)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tabs == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < tabs->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (tabs->tabs[index].active) return &tabs->tabs[index];
    }
    return NULL;
}
