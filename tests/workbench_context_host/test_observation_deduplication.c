/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_observation_deduplication.c
 *
 * PURPOSE:
 *   Verify an unchanged workbench observation does not create a second publication.
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
#include "umicom/workbench_context_host/host.h"

#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostObservation observation;
    uint64_t publications;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    umi_workbench_context_host_observation_init(&observation);
    assert(umi_workbench_context_host_copy_text(
        observation.document_id,
        sizeof(observation.document_id),
        "document") == UMI_STATUS_OK);
    observation.source_revision = 10U;
    observation.observed_at_ms = 100U;
    assert(umi_workbench_context_host_observe(
        fixture.host, &observation) == UMI_STATUS_OK);
    publications = fixture.host->metrics.publication_count;
    observation.observed_at_ms = 110U;
    assert(umi_workbench_context_host_observe(
        fixture.host, &observation) == UMI_STATUS_OK);
    assert(fixture.host->metrics.publication_count == publications);
    assert(fixture.host->metrics.duplicate_observation_count == 1U);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
