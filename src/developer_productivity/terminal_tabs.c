/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/terminal_tabs.c
 *
 * PURPOSE:
 *   Implement terminal tab ordering and active-state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/terminal_tabs.h"

#include <stdio.h>
#include <string.h>

static size_t find_index(
    const UmiDeveloperTerminalTabModel *model,
    const char *session_id)
{
    size_t index;

    if (model == NULL || session_id == NULL) return (size_t)-1;

    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->tabs[index].session_id, session_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

void umi_developer_terminal_tabs_init(
    UmiDeveloperTerminalTabModel *model)
{
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->revision = 1U;
}

UmiStatus umi_developer_terminal_tabs_add(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id,
    const char *title)
{
    UmiDeveloperTerminalTab *tab;

    if (model == NULL || session_id == NULL ||
        title == NULL || session_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (find_index(model, session_id) != (size_t)-1) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    if (model->count >= UMI_DEVELOPER_TERMINAL_SESSION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    tab = &model->tabs[model->count++];
    (void)snprintf(tab->session_id, sizeof(tab->session_id), "%s", session_id);
    (void)snprintf(tab->title, sizeof(tab->title), "%s", title);

    return umi_developer_terminal_tabs_activate(model, session_id);
}

UmiStatus umi_developer_terminal_tabs_activate(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id)
{
    size_t index;
    size_t target;

    if (model == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    target = find_index(model, session_id);
    if (target == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    for (index = 0U; index < model->count; ++index) {
        model->tabs[index].active = index == target;
    }

    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_terminal_tabs_remove(
    UmiDeveloperTerminalTabModel *model,
    const char *session_id)
{
    size_t index;
    size_t tail;
    int was_active;

    if (model == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(model, session_id);
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    was_active = model->tabs[index].active;
    tail = model->count - index - 1U;

    if (tail > 0U) {
        (void)memmove(
            &model->tabs[index],
            &model->tabs[index + 1U],
            tail * sizeof(model->tabs[0]));
    }

    model->count -= 1U;
    (void)memset(&model->tabs[model->count], 0, sizeof(model->tabs[0]));

    if (was_active && model->count > 0U) {
        model->tabs[index < model->count ? index : model->count - 1U].active = 1;
    }

    model->revision += 1U;
    return UMI_STATUS_OK;
}
