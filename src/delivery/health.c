/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/health.c
 *
 * PURPOSE:
 *   Summarise post-deployment readiness and liveness checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Health gates decide whether a generation should stay active or whether the release process should roll back.
 */

#include "umicom/delivery/health.h"
#include <string.h>
void umi_delivery_health_init(UmiDeliveryHealth *health)
{
    if (health != NULL) (void)memset(health, 0, sizeof(*health));
}
int umi_delivery_health_acceptable(const UmiDeliveryHealth *health)
{
    return health != NULL && health->ready != 0 && health->live != 0 &&
           health->checks_failed == 0U;
}
