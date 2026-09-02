/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_operations_health.c
 *
 * PURPOSE:
 *   Implement the test operations health behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Operations health tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/observability/health_summary.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiHealthRegistry *health = NULL;
    UmiReadinessRegistry *readiness = NULL;
    UmiOperationsHealthSummary summary;
    assert(umi_health_registry_create(&health) == UMI_STATUS_OK);
    assert(umi_readiness_registry_create(&readiness) == UMI_STATUS_OK);
    assert(umi_health_registry_update(health,"runtime",UMI_HEALTH_READY,"ready",1U) == UMI_STATUS_OK);
    assert(umi_health_registry_update(health,"adapter",UMI_HEALTH_DEGRADED,"reconnecting",2U) == UMI_STATUS_OK);
    assert(umi_readiness_registry_update(readiness,"database",0,"waiting",3U) == UMI_STATUS_OK);
    assert(umi_operations_health_summarise(health,readiness,&summary) == UMI_STATUS_OK);
    assert(summary.components == 2U);
    assert(summary.failed_readiness_checks == 1U);
    assert(summary.overall == UMI_OPERATIONS_HEALTH_DEGRADED);
    umi_readiness_registry_destroy(readiness);
    umi_health_registry_destroy(health);
    return 0;
}
