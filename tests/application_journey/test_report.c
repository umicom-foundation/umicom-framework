/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/test_report.c
 *
 * PURPOSE:
 *   Verify release readiness stays closed until all blocking journey steps carry passed evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/application/journey/journey_platform.h"
int main(void)
{
    UmiApplicationJourneyCatalogue catalogue;
    UmiApplicationJourneyEvidenceRegistry evidence;
    UmiApplicationJourneyReport report;
    assert(umi_application_journey_catalogue_build(&catalogue) == UMI_STATUS_OK);
    umi_application_journey_evidence_registry_init(&evidence);
    assert(umi_application_journey_report_build(
               &catalogue, &evidence, "org.umicom.studio", &report) ==
           UMI_STATUS_OK);
    assert(report.journey_count == 8U);
    assert(report.step_count == 40U);
    assert(report.pending_step_count == 40U);
    assert(report.completion_percent == 0U);
    assert(!report.release_ready);
    return 0;
}

