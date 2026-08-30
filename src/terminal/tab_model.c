/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/tab_model.c
 *
 * PURPOSE:
 *   Implement the tab model behavior for
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
 * Umicom Framework terminal tab model.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/tab_model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiTerminalTabModel {
    UmiTerminalTabSnapshot tabs[UMI_TERMINAL_TAB_MAX];
    size_t count;
    uint64_t revision;
};

static size_t tab_index(const UmiTerminalTabModel *model, const char *session_id)
{
    size_t index;
    if (model == NULL || session_id == NULL) return SIZE_MAX;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->tabs[index].session_id, session_id) == 0) return index;
    }
    return SIZE_MAX;
}

static void normalize(UmiTerminalTabModel *model)
{
    size_t index;
    for (index = 0U; index < model->count; ++index) model->tabs[index].order = index;
}

UmiStatus umi_terminal_tab_model_create(UmiTerminalTabModel **out_model)
{
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = (UmiTerminalTabModel *)calloc(1U, sizeof(**out_model));
    return *out_model != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_terminal_tab_model_destroy(UmiTerminalTabModel *model) { free(model); }

UmiStatus umi_terminal_tab_model_add(UmiTerminalTabModel *model,
                                     const char *session_id,
                                     const char *title,
                                     const char *profile_id)
{
    UmiTerminalTabSnapshot *tab;
    if (model == NULL || session_id == NULL || session_id[0] == '\0' ||
        title == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (tab_index(model, session_id) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS;
    if (model->count >= UMI_TERMINAL_TAB_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    tab = &model->tabs[model->count++];
    (void)memset(tab, 0, sizeof(*tab));
    (void)snprintf(tab->session_id, sizeof(tab->session_id), "%s", session_id);
    (void)snprintf(tab->title, sizeof(tab->title), "%s", title);
    (void)snprintf(tab->profile_id, sizeof(tab->profile_id), "%s", profile_id);
    tab->order = model->count - 1U;
    tab->active = model->count == 1U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_tab_model_remove(UmiTerminalTabModel *model,
                                        const char *session_id)
{
    size_t index = tab_index(model, session_id);
    int was_active;
    if (model == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    was_active = model->tabs[index].active;
    if (index + 1U < model->count) {
        (void)memmove(&model->tabs[index], &model->tabs[index + 1U],
                      (model->count - index - 1U) * sizeof(model->tabs[0]));
    }
    model->count -= 1U;
    if (was_active && model->count > 0U) {
        size_t active = index < model->count ? index : model->count - 1U;
        model->tabs[active].active = 1;
        model->tabs[active].unread_lines = 0U;
    }
    normalize(model);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_tab_model_activate(UmiTerminalTabModel *model,
                                          const char *session_id)
{
    size_t target = tab_index(model, session_id);
    size_t index;
    if (model == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (target == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < model->count; ++index) model->tabs[index].active = 0;
    model->tabs[target].active = 1;
    model->tabs[target].unread_lines = 0U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_tab_model_activate_relative(UmiTerminalTabModel *model,
                                                   int direction)
{
    size_t index;
    size_t active = SIZE_MAX;
    if (model == NULL || direction == 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->count == 0U) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < model->count; ++index) {
        if (model->tabs[index].active) { active = index; break; }
    }
    if (active == SIZE_MAX) active = 0U;
    if (direction > 0) active = (active + 1U) % model->count;
    else active = active == 0U ? model->count - 1U : active - 1U;
    return umi_terminal_tab_model_activate(model, model->tabs[active].session_id);
}

UmiStatus umi_terminal_tab_model_move(UmiTerminalTabModel *model,
                                      const char *session_id,
                                      size_t target_index)
{
    size_t source = tab_index(model, session_id);
    UmiTerminalTabSnapshot moved;
    if (model == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (source == SIZE_MAX || target_index >= model->count) return UMI_STATUS_NOT_FOUND;
    if (source == target_index) return UMI_STATUS_OK;
    moved = model->tabs[source];
    if (source < target_index) {
        (void)memmove(&model->tabs[source], &model->tabs[source + 1U],
                      (target_index - source) * sizeof(model->tabs[0]));
    } else {
        (void)memmove(&model->tabs[target_index + 1U], &model->tabs[target_index],
                      (source - target_index) * sizeof(model->tabs[0]));
    }
    model->tabs[target_index] = moved;
    normalize(model);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_tab_model_set_pinned(UmiTerminalTabModel *model,
                                            const char *session_id,
                                            int pinned)
{
    size_t index = tab_index(model, session_id);
    if (model == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    model->tabs[index].pinned = pinned != 0;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_tab_model_add_unread(UmiTerminalTabModel *model,
                                            const char *session_id,
                                            uint64_t lines)
{
    size_t index = tab_index(model, session_id);
    if (model == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (!model->tabs[index].active) model->tabs[index].unread_lines += lines;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_tab_model_at(const UmiTerminalTabModel *model,
                                    size_t index,
                                    UmiTerminalTabSnapshot *out_tab)
{
    if (model == NULL || out_tab == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_tab = model->tabs[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_tab_model_find(const UmiTerminalTabModel *model,
                                      const char *session_id,
                                      UmiTerminalTabSnapshot *out_tab)
{
    size_t index;
    if (model == NULL || session_id == NULL || out_tab == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = tab_index(model, session_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_tab = model->tabs[index];
    return UMI_STATUS_OK;
}

size_t umi_terminal_tab_model_count(const UmiTerminalTabModel *model)
{ return model != NULL ? model->count : 0U; }

const char *umi_terminal_tab_model_active_id(const UmiTerminalTabModel *model)
{
    size_t index;
    if (model == NULL) return NULL;
    for (index = 0U; index < model->count; ++index) {
        if (model->tabs[index].active) return model->tabs[index].session_id;
    }
    return NULL;
}

uint64_t umi_terminal_tab_model_revision(const UmiTerminalTabModel *model)
{ return model != NULL ? model->revision : 0U; }
