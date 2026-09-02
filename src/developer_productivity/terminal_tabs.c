/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/terminal_tabs.c
 *
 * PURPOSE:
 *   Implement terminal tab ordering and active-state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/terminal_tabs.h"

#include <stdio.h>
#include <string.h>

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(
    const UmiDeveloperTerminalTabModel *model,
    const char *session_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || session_id == NULL) return (size_t)-1;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->tabs[index].session_id, session_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

/*
 * Initialise developer terminal tabs from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_terminal_tabs_init(
    UmiDeveloperTerminalTabModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->revision = 1U;
}

/*
 * Add developer terminal tabs only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_developer_terminal_tabs_add(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id,
    const char *title)
{
    UmiDeveloperTerminalTab *tab;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || session_id == NULL ||
        title == NULL || session_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_index(model, session_id) != (size_t)-1) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_DEVELOPER_TERMINAL_SESSION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    tab = &model->tabs[model->count++];
    (void)snprintf(tab->session_id, sizeof(tab->session_id), "%s", session_id);
    (void)snprintf(tab->title, sizeof(tab->title), "%s", title);

    return umi_developer_terminal_tabs_activate(model, session_id);
}

/*
 * Provide the developer terminal tabs activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_terminal_tabs_activate(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id)
{
    size_t index;
    size_t target;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    target = find_index(model, session_id);
    /* Configure the optional target only when its feature has created it. */
    if (target == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        model->tabs[index].active = index == target;
    }

    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove developer terminal tabs while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_developer_terminal_tabs_remove(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id)
{
    size_t index;
    size_t tail;
    int was_active;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(model, session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    was_active = model->tabs[index].active;
    tail = model->count - index - 1U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (tail > 0U) {
        (void)memmove(
            &model->tabs[index],
            &model->tabs[index + 1U],
            tail * sizeof(model->tabs[0]));
    }

    model->count -= 1U;
    (void)memset(&model->tabs[model->count], 0, sizeof(model->tabs[0]));

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (was_active && model->count > 0U) {
        model->tabs[index < model->count ? index : model->count - 1U].active = 1;
    }

    model->revision += 1U;
    return UMI_STATUS_OK;
}
