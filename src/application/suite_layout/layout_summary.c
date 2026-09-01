/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/layout_summary.c
 *
 * PURPOSE:
 *   Calculate one trustworthy layout summary from canonical application
 *   experience metadata for selectors, catalogues and frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/layout_summary.h"

#include <string.h>

/* Add one panel to the correct region counter. Keeping this decision in one
 * helper prevents selectors and catalogues from interpreting regions
 * differently. */
static UmiStatus count_panel_region(
    const UmiExperiencePanelDefinition *panel,
    UmiApplicationSuiteLayoutSummary *summary)
{
    UmiUiPlacement placement;
    UmiStatus status;

    /* Both objects are required because this helper reads one and updates the
     * other. Rejecting null here prevents a malformed catalogue from being
     * reported as a valid but empty layout. */
    if (panel == NULL || summary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_placement_parse(panel->default_region, &placement);
    /* An unknown region cannot be guessed safely because that would place the
     * panel differently in different frontend adapters. */
    if (status != UMI_STATUS_OK) return status;

    /* Each valid placement contributes to exactly one counter. This makes the
     * final total a simple and useful completeness check. */
    switch (placement) {
    case UMI_UI_PLACEMENT_LEFT: summary->left_count += 1U; break;
    case UMI_UI_PLACEMENT_CENTRE: summary->centre_count += 1U; break;
    case UMI_UI_PLACEMENT_RIGHT: summary->right_count += 1U; break;
    case UMI_UI_PLACEMENT_TOP: summary->top_count += 1U; break;
    case UMI_UI_PLACEMENT_BOTTOM: summary->bottom_count += 1U; break;
    case UMI_UI_PLACEMENT_FLOATING: summary->floating_count += 1U; break;
    default: return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* Build a lightweight view over existing catalogue records. No layout or
 * panel strings are copied, allocated or given new ownership here. */
UmiStatus umi_application_suite_layout_summary_build(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperienceLayoutDefinition *layout,
    UmiApplicationSuiteLayoutSummary *out_summary)
{
    const UmiExperienceLayoutDefinition *registered_layout;
    size_t index;
    size_t placed_count;
    UmiStatus status;

    /* Validation needs the owner, the chosen layout and writable result
     * storage. No partial summary is returned when one is missing. */
    if (experience == NULL || layout == NULL || out_summary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_experience_validate(experience);
    /* Invalid experience metadata makes every nested layout unreliable, so
     * stop before borrowing any of its pointers. */
    if (status != UMI_STATUS_OK) return status;
    registered_layout = umi_application_experience_layout_find(
        experience, layout->layout_id);
    /* A matching identifier from another object is still foreign metadata.
     * Requiring the registered address protects the borrowed lifetime rule. */
    if (registered_layout != layout) return UMI_STATUS_NOT_FOUND;

    (void)memset(out_summary, 0, sizeof(*out_summary));
    out_summary->definition = layout;
    out_summary->panel_count = layout->panel_count;
    out_summary->default_layout =
        strcmp(layout->layout_id, experience->default_layout_id) == 0;
    out_summary->lockable =
        (layout->flags & UMI_EXPERIENCE_LAYOUT_LOCKABLE) != 0U;
    out_summary->multi_monitor =
        (layout->flags & UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR) != 0U;
    out_summary->responsive =
        (layout->flags & UMI_EXPERIENCE_LAYOUT_RESPONSIVE) != 0U;
    out_summary->context_linked =
        (layout->flags & UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED) != 0U;

    /* Resolve every stable panel identifier through the owning experience.
     * This proves the recipe never refers to a missing application panel. */
    for (index = 0U; index < layout->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel =
            umi_application_experience_panel_find(
                experience, layout->panel_ids[index]);
        /* A missing panel makes the entire named layout incomplete. */
        if (panel == NULL) return UMI_STATUS_NOT_FOUND;
        status = count_panel_region(panel, out_summary);
        /* Preserve the exact placement error so a caller can diagnose the
         * canonical record rather than receiving a generic failure. */
        if (status != UMI_STATUS_OK) return status;
    }

    placed_count = out_summary->left_count + out_summary->centre_count +
        out_summary->right_count + out_summary->top_count +
        out_summary->bottom_count + out_summary->floating_count;
    /* A useful layout contains at least one panel, and every panel must have
     * contributed to exactly one valid region. */
    out_summary->complete =
        layout->panel_count > 0U && placed_count == layout->panel_count;
    return out_summary->complete ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
