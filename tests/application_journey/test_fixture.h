/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/test_fixture.h
 *
 * PURPOSE:
 *   Share deterministic journey validation and pass-path helpers across focused scenario tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_JOURNEY_TEST_FIXTURE_H
#define UMICOM_APPLICATION_JOURNEY_TEST_FIXTURE_H
#include <assert.h>
#include <string.h>
#include "umicom/application/journey/journey_platform.h"
/**
 * Exercise verify journey and return a clear result when the behaviour no longer matches
 * its contract.
 */
static void verify_journey(const char *journey_id, const char *application_id,
                           const char *layout_id)
{
    UmiApplicationJourneyCatalogue catalogue;
    UmiApplicationJourneyAudit audit;
    UmiApplicationJourneyEvidenceRegistry evidence;
    UmiApplicationJourneyRuntime runtime;
    UmiApplicationJourneyReport report;
    const UmiApplicationJourney *journey;
    size_t index = 0U;
    assert(umi_application_journey_catalogue_build(&catalogue) == UMI_STATUS_OK);
    journey = umi_application_journey_catalogue_find(&catalogue, journey_id);
    assert(journey != NULL);
    assert(strcmp(journey->application_id, application_id) == 0);
    assert(strcmp(journey->layout_id, layout_id) == 0);
    assert(journey->step_count == 5U);
    assert(umi_application_journey_audit_catalogue(&catalogue, &audit) ==
           UMI_STATUS_OK);
    assert(audit.valid);
    umi_application_journey_evidence_registry_init(&evidence);
    assert(umi_application_journey_runtime_begin(
               &runtime, journey, &evidence) == UMI_STATUS_OK);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (umi_application_journey_runtime_current(&runtime) != NULL) {
        assert(umi_application_journey_runtime_record(
                   &runtime, UMI_APPLICATION_JOURNEY_PASSED,
                   "accepted", 1000U + index) == UMI_STATUS_OK);
        index++;
    }
    assert(runtime.complete);
    assert(runtime.result == UMI_APPLICATION_JOURNEY_PASSED);
    assert(runtime.passed_step_count == journey->step_count);
    assert(umi_application_journey_report_build(
               &catalogue, &evidence, application_id, &report) == UMI_STATUS_OK);
    assert(report.journey_count == 8U);
    assert(report.step_count == 40U);
    assert(report.passed_step_count == 5U);
}
#endif

