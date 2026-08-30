/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_supervisor.c
 *
 * PURPOSE:
 *   Verify supervised component failure classification and bounded restart
 *   decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include "umicom/resilience/supervisor.h"
int main(void) {
    UmiResilienceSupervisor *s = NULL; UmiRestartDecision d; UmiSupervisedSnapshot snap;
    assert(umi_resilience_supervisor_create(&s) == UMI_STATUS_OK);
    assert(umi_resilience_supervisor_register(s,"worker.index",NULL,0U) == UMI_STATUS_OK);
    assert(umi_resilience_supervisor_failure(s,"worker.index",UMI_STATUS_UNAVAILABLE,10U,&d) == UMI_STATUS_OK && d.restart);
    assert(umi_resilience_supervisor_at(s,0U,&snap) == UMI_STATUS_OK && snap.restarts == 1U);
    assert(umi_resilience_supervisor_success(s,"worker.index",20U) == UMI_STATUS_OK);
    umi_resilience_supervisor_destroy(s); return 0;
}
