/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_health.c
 *
 * PURPOSE:
 *   Verify component health snapshots, sequence updates and aggregate
 *   readiness, degradation and failure calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiHealthRegistry *registry = NULL;
    UmiHealthSnapshot snapshot;

    assert(umi_health_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_health_registry_overall(registry) == UMI_HEALTH_UNKNOWN);

    assert(umi_health_registry_update(registry,
                                      "module.one",
                                      UMI_HEALTH_READY,
                                      "ready",
                                      10U) == UMI_STATUS_OK);
    assert(umi_health_registry_update(registry,
                                      "module.two",
                                      UMI_HEALTH_DEGRADED,
                                      "waiting for optional provider",
                                      20U) == UMI_STATUS_OK);
    assert(umi_health_registry_count(registry) == 2U);
    assert(umi_health_registry_overall(registry) == UMI_HEALTH_DEGRADED);

    assert(umi_health_registry_get(registry,
                                   "module.two",
                                   &snapshot) == UMI_STATUS_OK);
    assert(snapshot.state == UMI_HEALTH_DEGRADED);
    assert(strcmp(snapshot.message, "waiting for optional provider") == 0);

    assert(umi_health_registry_update(registry,
                                      "module.two",
                                      UMI_HEALTH_FAILED,
                                      "failed",
                                      30U) == UMI_STATUS_OK);
    assert(umi_health_registry_overall(registry) == UMI_HEALTH_FAILED);

    umi_health_registry_destroy(registry);
    return 0;
}
