/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_readiness.c
 *
 * PURPOSE:
 *   Verify readiness checks and aggregate readiness state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include "umicom/diagnostics/readiness.h"
int main(void) {
    UmiReadinessRegistry *r = NULL;
    assert(umi_readiness_registry_create(&r) == UMI_STATUS_OK);
    assert(umi_readiness_registry_update(r, "data", 1, "ready", 1U) == UMI_STATUS_OK);
    assert(umi_readiness_registry_ready(r));
    assert(umi_readiness_registry_update(r, "broker", 0, "offline", 2U) == UMI_STATUS_OK);
    assert(!umi_readiness_registry_ready(r));
    umi_readiness_registry_destroy(r); return 0;
}
