/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_deployment_health.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/deployment.h"
#include "umicom/delivery/health.h"
int main(void) {
    UmiDeployment deployment;
    UmiDeliveryHealth health;
    assert(umi_deployment_init(&deployment, "d1", "r1", "local", 2U) == UMI_STATUS_OK);
    umi_delivery_health_init(&health);
    health.ready = 1; health.live = 1; health.checks_passed = 2U;
    assert(umi_delivery_health_acceptable(&health));
    return 0;
}
