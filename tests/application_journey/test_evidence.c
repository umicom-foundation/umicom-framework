/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/test_evidence.c
 *
 * PURPOSE:
 *   Verify evidence replacement is bounded, revisioned and addressable by journey and step.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/application/journey/evidence.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationJourneyEvidenceRegistry registry;
    const UmiApplicationJourneyEvidenceRecord *record;
    umi_application_journey_evidence_registry_init(&registry);
    assert(umi_application_journey_evidence_record(
               &registry, "journey", "step", UMI_APPLICATION_JOURNEY_PASSED,
               "first", 10U) == UMI_STATUS_OK);
    assert(umi_application_journey_evidence_record(
               &registry, "journey", "step", UMI_APPLICATION_JOURNEY_FAILED,
               "updated", 20U) == UMI_STATUS_OK);
    assert(registry.count == 1U);
    assert(registry.revision == 2U);
    record = umi_application_journey_evidence_find(
        &registry, "journey", "step");
    assert(record != NULL);
    assert(record->result == UMI_APPLICATION_JOURNEY_FAILED);
    assert(strcmp(record->note, "updated") == 0);
    return 0;
}

