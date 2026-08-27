/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/audit.c
 *
 * PURPOSE:
 *   Cross-check every acceptance journey against the canonical experience catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/journey/audit.h"
#include <string.h>
#include "umicom/application/experience_catalogue.h"
UmiStatus umi_application_journey_audit_catalogue(
    const UmiApplicationJourneyCatalogue *catalogue,
    UmiApplicationJourneyAudit *out_audit)
{
    size_t journey_index;
    if (catalogue == NULL || out_audit == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_audit, 0, sizeof(*out_audit));
    for (journey_index = 0U; journey_index < catalogue->count; ++journey_index) {
        const UmiApplicationJourney *journey = catalogue->entries[journey_index];
        const UmiApplicationExperienceDefinition *experience;
        size_t step_index;
        out_audit->journey_count++;
        if (journey == NULL ||
            umi_application_journey_validate(journey) != UMI_STATUS_OK) {
            out_audit->invalid_journey_count++;
            continue;
        }
        if (journey->blocks_release) out_audit->blocking_journey_count++;
        experience = umi_application_experience_catalogue_find(
            journey->application_id);
        if (experience == NULL) {
            out_audit->missing_application_count++;
            continue;
        }
        if (umi_application_experience_layout_find(
                experience, journey->layout_id) == NULL)
            out_audit->missing_layout_count++;
        for (step_index = 0U; step_index < journey->step_count; ++step_index) {
            const UmiApplicationJourneyStep *step = &journey->steps[step_index];
            out_audit->step_count++;
            if (step->critical) out_audit->critical_step_count++;
            if (step->requires_confirmation)
                out_audit->confirmation_step_count++;
            if (step->command_id == NULL || step->command_id[0] == '\0')
                out_audit->invalid_command_count++;
            if (step->panel_id != NULL && step->panel_id[0] != '\0' &&
                umi_application_experience_panel_find(
                    experience, step->panel_id) == NULL)
                out_audit->missing_panel_count++;
        }
    }
    out_audit->valid =
        out_audit->invalid_journey_count == 0U &&
        out_audit->missing_application_count == 0U &&
        out_audit->missing_layout_count == 0U &&
        out_audit->missing_panel_count == 0U &&
        out_audit->invalid_command_count == 0U;
    return out_audit->valid ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

