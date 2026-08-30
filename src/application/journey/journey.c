/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journey.c
 *
 * PURPOSE:
 *   Validate bounded product acceptance journey contracts and expose stable diagnostic text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/journey/journey.h"
#include <string.h>
static int text_present(const char *text) { return text != NULL && text[0] != '\0'; }
UmiStatus umi_application_journey_validate(const UmiApplicationJourney *journey)
{
    size_t index;
    size_t other;
    if (journey == NULL ||
        journey->structure_size != sizeof(UmiApplicationJourney) ||
        journey->api_version != UMI_APPLICATION_JOURNEY_API_VERSION ||
        !text_present(journey->application_id) ||
        !text_present(journey->journey_id) ||
        !text_present(journey->title) ||
        !text_present(journey->layout_id) ||
        journey->steps == NULL || journey->step_count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < journey->step_count; ++index) {
        const UmiApplicationJourneyStep *step = &journey->steps[index];
        if (step->structure_size != sizeof(UmiApplicationJourneyStep) ||
            !text_present(step->step_id) || !text_present(step->title) ||
            !text_present(step->command_id) ||
            step->evidence_kind < UMI_APPLICATION_JOURNEY_EVIDENCE_STATE ||
            step->evidence_kind > UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (other = index + 1U; other < journey->step_count; ++other)
            if (strcmp(step->step_id, journey->steps[other].step_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}
const char *umi_application_journey_evidence_kind_text(
    UmiApplicationJourneyEvidenceKind kind)
{
    switch (kind) {
        case UMI_APPLICATION_JOURNEY_EVIDENCE_STATE: return "state";
        case UMI_APPLICATION_JOURNEY_EVIDENCE_COMMAND: return "command";
        case UMI_APPLICATION_JOURNEY_EVIDENCE_PANEL: return "panel";
        case UMI_APPLICATION_JOURNEY_EVIDENCE_LAYOUT: return "layout";
        case UMI_APPLICATION_JOURNEY_EVIDENCE_RECOVERY: return "recovery";
        case UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY: return "safety";
        default: return "unknown";
    }
}
const char *umi_application_journey_result_text(
    UmiApplicationJourneyResult result)
{
    switch (result) {
        case UMI_APPLICATION_JOURNEY_PENDING: return "pending";
        case UMI_APPLICATION_JOURNEY_PASSED: return "passed";
        case UMI_APPLICATION_JOURNEY_FAILED: return "failed";
        case UMI_APPLICATION_JOURNEY_BLOCKED: return "blocked";
        default: return "unknown";
    }
}

