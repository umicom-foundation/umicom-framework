/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/selector_model.c
 *
 * PURPOSE:
 *   Build and update frontend-neutral layout choices from canonical product
 *   experience metadata so native and browser adapters share selection rules.
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

/* Copy selector text into fixed storage and report truncation as a contract
 * error instead of leaving a partly written label. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /* All selector labels are owned fixed strings, so the helper needs a real
     * source and a destination with at least one byte of storage. */
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    /* A shortened title or identifier would misrepresent the canonical
     * choice; report capacity failure instead. */
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

/* Accept an unqualified layout ID or its application-qualified saved form. */
static int active_matches(const UmiApplicationExperienceDefinition *experience,
                          const char *active,
                          const char *layout_id)
{
    char qualified[UMI_UI_ID_CAPACITY];
    int written;
    /* No saved choice means first launch, so select the canonical default. */
    if (active == NULL || active[0] == '\0')
        return strcmp(experience->default_layout_id, layout_id) == 0;
    /* Frontends may hold the short identifier while persistence uses the
     * application-qualified form; both describe the same layout. */
    if (strcmp(active, layout_id) == 0) return 1;
    written = snprintf(qualified, sizeof(qualified), "%s.%s",
                       experience->application_id, layout_id);
    return written >= 0 && (size_t)written < sizeof(qualified) &&
           strcmp(active, qualified) == 0;
}

/* Create choices and guidance for every canonical layout in one application. */
UmiStatus umi_application_suite_layout_selector_build(
    const UmiApplicationExperienceDefinition *experience,
    const char *active_layout_id,
    UmiApplicationSuiteLayoutSelectorModel *out_model)
{
    size_t index;
    int selected = 0;
    UmiStatus status;
    /* Canonical choices and writable model storage are both required. */
    if (experience == NULL || out_model == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_experience_validate(experience);
    /* Invalid experience relationships must not reach a layout menu. */
    if (status != UMI_STATUS_OK) return status;
    /* A selector needs at least one choice and may not exceed its bounded
     * owned array. */
    if (experience->layout_count == 0U ||
        experience->layout_count > UMI_APPLICATION_SUITE_LAYOUT_SELECTOR_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(out_model, 0, sizeof(*out_model));
    /* Convert every canonical definition into display-ready owned metadata. */
    for (index = 0U; index < experience->layout_count; ++index) {
        const UmiExperienceLayoutDefinition *layout = &experience->layouts[index];
        UmiApplicationSuiteLayoutChoice *choice = &out_model->choices[index];
        UmiApplicationSuiteLayoutSummary summary;
        status = umi_application_suite_layout_summary_build(
            experience, layout, &summary);
        /* A choice is offered only after its whole panel recipe validates. */
        if (status != UMI_STATUS_OK) return status;
        status = copy_text(choice->layout_id, sizeof(choice->layout_id),
                           layout->layout_id);
        /* Continue copying only while earlier required text remains complete. */
        if (status == UMI_STATUS_OK)
            status = copy_text(choice->title, sizeof(choice->title), layout->title);
        /* The description is copied last because it has the largest storage
         * and is not useful when the identifier or title already failed. */
        if (status == UMI_STATUS_OK)
            status = copy_text(choice->description, sizeof(choice->description),
                               layout->description);
        /* Never publish a partly copied choice. */
        if (status != UMI_STATUS_OK) return status;
        /* Copy small behavioural facts into the frontend-neutral choice. A UI
         * can now explain a layout before applying it without owning logic. */
        choice->panel_count = summary.panel_count;
        choice->default_layout = summary.default_layout;
        choice->lockable = summary.lockable;
        choice->multi_monitor = summary.multi_monitor;
        choice->responsive = summary.responsive;
        choice->context_linked = summary.context_linked;
        choice->selected = active_matches(experience, active_layout_id,
                                          layout->layout_id);
        /* Exactly one record may match the active layout. Two matches would
         * make later save and apply commands ambiguous. */
        if (choice->selected) {
            /* Detect a duplicate match before replacing the first index. */
            if (selected) return UMI_STATUS_INVALID_STATE;
            out_model->selected_index = index;
            selected = 1;
        }
    }
    /* An unknown saved identifier is reported instead of silently changing
     * the user's workspace to another layout. */
    if (!selected) return UMI_STATUS_NOT_FOUND;
    out_model->count = experience->layout_count;
    out_model->revision = 1U;
    return UMI_STATUS_OK;
}

/* Select one known choice while preserving the one-selected-item invariant. */
UmiStatus umi_application_suite_layout_selector_select(
    UmiApplicationSuiteLayoutSelectorModel *model,
    const char *layout_id)
{
    size_t index;
    /* A missing model or empty stable identifier cannot select a choice. */
    if (model == NULL || layout_id == NULL || layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Search bounded owned choices; unused capacity must never participate. */
    for (index = 0U; index < model->count; ++index) {
        /* Non-matching entries remain unchanged while the search continues. */
        if (strcmp(model->choices[index].layout_id, layout_id) != 0) continue;
        /* Selecting the existing choice is intentionally a no-op so observers
         * do not receive a false revision. */
        if (model->selected_index != index) {
            size_t nested;
            /* Rebuild every selection flag to preserve the exactly-one rule. */
            for (nested = 0U; nested < model->count; ++nested)
                model->choices[nested].selected = nested == index;
            model->selected_index = index;
            model->revision += 1U;
        }
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Return the currently selected borrowed choice, or NULL for an invalid model. */
const UmiApplicationSuiteLayoutChoice *
umi_application_suite_layout_selector_current(
    const UmiApplicationSuiteLayoutSelectorModel *model)
{
    return model != NULL && model->selected_index < model->count
        ? &model->choices[model->selected_index] : NULL;
}

/* Return one borrowed choice by bounded index. */
const UmiApplicationSuiteLayoutChoice *
umi_application_suite_layout_selector_at(
    const UmiApplicationSuiteLayoutSelectorModel *model,
    size_t index)
{
    return model != NULL && index < model->count
        ? &model->choices[index] : NULL;
}
