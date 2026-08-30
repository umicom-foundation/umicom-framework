/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/selector_model.c
 *
 * PURPOSE:
 *   Build and update frontend-neutral layout choices from canonical product
 *   experience metadata so GTK4, Qt and Wt can share selection semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/selector_model.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static int active_matches(const UmiApplicationExperienceDefinition *experience,
                          const char *active,
                          const char *layout_id)
{
    char qualified[UMI_UI_ID_CAPACITY];
    int written;
    if (active == NULL || active[0] == '\0')
        return strcmp(experience->default_layout_id, layout_id) == 0;
    if (strcmp(active, layout_id) == 0) return 1;
    written = snprintf(qualified, sizeof(qualified), "%s.%s",
                       experience->application_id, layout_id);
    return written >= 0 && (size_t)written < sizeof(qualified) &&
           strcmp(active, qualified) == 0;
}

UmiStatus umi_application_suite_layout_selector_build(
    const UmiApplicationExperienceDefinition *experience,
    const char *active_layout_id,
    UmiApplicationSuiteLayoutSelectorModel *out_model)
{
    size_t index;
    int selected = 0;
    UmiStatus status;
    if (experience == NULL || out_model == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_experience_validate(experience);
    if (status != UMI_STATUS_OK) return status;
    if (experience->layout_count == 0U ||
        experience->layout_count > UMI_APPLICATION_SUITE_LAYOUT_SELECTOR_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(out_model, 0, sizeof(*out_model));
    for (index = 0U; index < experience->layout_count; ++index) {
        const UmiExperienceLayoutDefinition *layout = &experience->layouts[index];
        UmiApplicationSuiteLayoutChoice *choice = &out_model->choices[index];
        status = copy_text(choice->layout_id, sizeof(choice->layout_id),
                           layout->layout_id);
        if (status == UMI_STATUS_OK)
            status = copy_text(choice->title, sizeof(choice->title), layout->title);
        if (status == UMI_STATUS_OK)
            status = copy_text(choice->description, sizeof(choice->description),
                               layout->description);
        if (status != UMI_STATUS_OK) return status;
        choice->selected = active_matches(experience, active_layout_id,
                                          layout->layout_id);
        if (choice->selected) {
            if (selected) return UMI_STATUS_INVALID_STATE;
            out_model->selected_index = index;
            selected = 1;
        }
    }
    if (!selected) return UMI_STATUS_NOT_FOUND;
    out_model->count = experience->layout_count;
    out_model->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_suite_layout_selector_select(
    UmiApplicationSuiteLayoutSelectorModel *model,
    const char *layout_id)
{
    size_t index;
    if (model == NULL || layout_id == NULL || layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->choices[index].layout_id, layout_id) != 0) continue;
        if (model->selected_index != index) {
            size_t nested;
            for (nested = 0U; nested < model->count; ++nested)
                model->choices[nested].selected = nested == index;
            model->selected_index = index;
            model->revision += 1U;
        }
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

const UmiApplicationSuiteLayoutChoice *
umi_application_suite_layout_selector_current(
    const UmiApplicationSuiteLayoutSelectorModel *model)
{
    return model != NULL && model->selected_index < model->count
        ? &model->choices[model->selected_index] : NULL;
}

const UmiApplicationSuiteLayoutChoice *
umi_application_suite_layout_selector_at(
    const UmiApplicationSuiteLayoutSelectorModel *model,
    size_t index)
{
    return model != NULL && index < model->count
        ? &model->choices[index] : NULL;
}
