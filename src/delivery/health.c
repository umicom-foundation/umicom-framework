/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/health.c
 *
 * PURPOSE:
 *   Summarise post-deployment readiness and liveness checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Health gates decide whether a generation should stay active or whether the release process should roll back.
 */

#include "umicom/delivery/health.h"
#include <string.h>
/*
 * Initialise delivery health from caller-provided values so later operations receive a
 * known state.
 */
void umi_delivery_health_init(UmiDeliveryHealth *health)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (health != NULL) (void)memset(health, 0, sizeof(*health));
}
/*
 * Provide the delivery health acceptable operation used by this module and its client
 * applications.
 */
int umi_delivery_health_acceptable(const UmiDeliveryHealth *health)
{
    return health != NULL && health->ready != 0 && health->live != 0 &&
           health->checks_failed == 0U;
}
