/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_observer.c
 *
 * PURPOSE:
 *   Verify duplicate workbench observations are counted and suppressed.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "test_support.h"

int main(void)
{

    UmiWorkbenchContextHostObserver observer;
    UmiWorkbenchContextHostObservation observation;
    umi_workbench_context_host_observer_init(&observer);
    umi_workbench_context_host_observation_init(&observation);
    assert(umi_workbench_context_host_copy_text(
        observation.document_id, sizeof(observation.document_id), "doc") == UMI_STATUS_OK);
    observation.source_revision = 1U;
    assert(umi_workbench_context_host_observer_push(
        &observer, &observation) == UINT32_MAX);
    assert(umi_workbench_context_host_observer_push(
        &observer, &observation) == 0U);
    assert(observer.observation_count == 2U);
    assert(observer.duplicate_count == 1U);
    return 0;
}
