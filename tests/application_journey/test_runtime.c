/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/test_runtime.c
 *
 * PURPOSE:
 *   Verify critical journey failures block progress and preserve failure evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/application/journey/journey_platform.h"
int main(void)
{
    UmiApplicationJourneyCatalogue catalogue;
    UmiApplicationJourneyEvidenceRegistry evidence;
    UmiApplicationJourneyRuntime runtime;
    const UmiApplicationJourney *journey;
    assert(umi_application_journey_catalogue_build(&catalogue) == UMI_STATUS_OK);
    journey = umi_application_journey_catalogue_find(
        &catalogue, "trader.live-release-gate");
    assert(journey != NULL);
    umi_application_journey_evidence_registry_init(&evidence);
    assert(umi_application_journey_runtime_begin(
               &runtime, journey, &evidence) == UMI_STATUS_OK);
    assert(umi_application_journey_runtime_record(
               &runtime, UMI_APPLICATION_JOURNEY_FAILED,
               "paper evidence missing", 10U) == UMI_STATUS_OK);
    assert(runtime.result == UMI_APPLICATION_JOURNEY_BLOCKED);
    assert(!runtime.active);
    assert(!runtime.complete);
    assert(runtime.failed_step_count == 1U);
    return 0;
}

