/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/descriptor.c
 *
 * PURPOSE:
 *   Build application layout coverage directly from canonical experience panel and layout metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/suite_layout/descriptor.h"

#include <stdio.h>
#include <string.h>

/* Copy public text into a bounded destination and reject truncation. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /* A missing buffer, empty buffer or missing source cannot produce a
     * complete public identifier. */
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    /* Treat truncation as an error because shortened identifiers could refer
     * to a different saved layout. */
    if (written < 0 || (size_t)written >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/* Build one application descriptor from all of its canonical layouts. */
UmiStatus umi_application_suite_layout_descriptor_build(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationSuiteLayoutDescriptor *out_descriptor)
{
    const UmiApplicationSuiteLayoutSummary *default_summary = NULL;
    size_t index;
    UmiStatus status;
    /* The operation needs canonical source metadata and writable owned
     * storage; neither may be inferred by the Framework. */
    if (experience == NULL || out_descriptor == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_experience_validate(experience);
    /* Do not copy facts from an experience whose internal relationships have
     * already failed validation. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_descriptor, 0, sizeof(*out_descriptor));
    status = copy_text(out_descriptor->application_id,
                       sizeof(out_descriptor->application_id),
                       experience->application_id);
    /* Return a precise copy error instead of leaving a partial application
     * identifier in the descriptor. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_descriptor->display_name,
                       sizeof(out_descriptor->display_name),
                       experience->display_name);
    /* Display text is part of the owned descriptor and must fit completely. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_descriptor->default_layout_id,
                       sizeof(out_descriptor->default_layout_id),
                       experience->default_layout_id);
    /* The saved default identifier must never be shortened or guessed. */
    if (status != UMI_STATUS_OK) return status;
    out_descriptor->panel_count = experience->panel_count;
    out_descriptor->layout_count = experience->layout_count;
    /* Fixed storage keeps this contract allocation-free. Refuse experiences
     * that exceed it instead of writing past the summary array. */
    if (experience->layout_count > UMI_APPLICATION_SUITE_LAYOUT_MAX_LAYOUTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Summarise every layout, not only the default. This ensures alternative
     * role, review and focused workspaces are real usable recipes too. */
    for (index = 0U; index < experience->layout_count; ++index) {
        UmiApplicationSuiteLayoutSummary *summary =
            &out_descriptor->layouts[out_descriptor->summary_count];
        status = umi_application_suite_layout_summary_build(
            experience, &experience->layouts[index], summary);
        /* One invalid alternative makes the application coverage incomplete;
         * count it before returning so diagnostics retain that fact. */
        if (status != UMI_STATUS_OK) {
            out_descriptor->invalid_layout_count += 1U;
            return status;
        }
        out_descriptor->summary_count += 1U;
        out_descriptor->valid_layout_count += 1U;
        out_descriptor->lockable_layout_count += (size_t)summary->lockable;
        out_descriptor->multi_monitor_layout_count +=
            (size_t)summary->multi_monitor;
        out_descriptor->responsive_layout_count +=
            (size_t)summary->responsive;
        out_descriptor->context_linked_layout_count +=
            (size_t)summary->context_linked;
        /* Remember the canonical default so legacy counters can still be
         * populated after every alternative has been validated. */
        if (summary->default_layout) default_summary = summary;
    }

    /* A missing default is left for the final completeness rule to reject. */
    if (default_summary != NULL) {
        /* Preserve the original default-layout counters for existing callers
         * while sourcing them from the shared per-layout summary contract. */
        out_descriptor->default_layout_valid = default_summary->complete;
        out_descriptor->default_panel_count = default_summary->panel_count;
        out_descriptor->left_count = default_summary->left_count;
        out_descriptor->centre_count = default_summary->centre_count;
        out_descriptor->right_count = default_summary->right_count;
        out_descriptor->top_count = default_summary->top_count;
        out_descriptor->bottom_count = default_summary->bottom_count;
        out_descriptor->floating_count = default_summary->floating_count;
    }
    out_descriptor->complete = out_descriptor->default_layout_valid &&
        out_descriptor->summary_count == experience->layout_count &&
        out_descriptor->valid_layout_count == experience->layout_count &&
        out_descriptor->invalid_layout_count == 0U;
    return out_descriptor->complete ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/* Search summaries already owned by the descriptor. The returned pointer is
 * borrowed and remains valid only while the descriptor remains alive. */
const UmiApplicationSuiteLayoutSummary *
umi_application_suite_layout_descriptor_find_layout(
    const UmiApplicationSuiteLayoutDescriptor *descriptor,
    const char *layout_id)
{
    size_t index;

    /* An empty search key cannot name a persisted layout, so it is handled in
     * the same safe way as a missing descriptor. */
    if (descriptor == NULL || layout_id == NULL || layout_id[0] == '\0')
        return NULL;
    /* Search only summaries that were successfully built into the owned
     * descriptor; unused fixed-capacity entries remain outside this range. */
    for (index = 0U; index < descriptor->summary_count; ++index) {
        const UmiApplicationSuiteLayoutSummary *summary =
            &descriptor->layouts[index];
        /* A summary must still have a borrowed definition before its stable
         * identifier can be compared. */
        if (summary->definition != NULL &&
            strcmp(summary->definition->layout_id, layout_id) == 0)
            return summary;
    }
    return NULL;
}
