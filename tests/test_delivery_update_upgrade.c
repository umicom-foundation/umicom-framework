/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_update_upgrade.c
 *
 * PURPOSE:
 *   Verify staged update channels and authorised rollback-safe upgrades.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/delivery/delivery.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUpdateChannel channel;
    UmiUpgradePlan upgrade;
    assert(umi_update_channel_init(
               &channel, "stable", "https://updates.umicom.org/studio/stable.json",
               UMI_RELEASE_STABLE, 25U) == UMI_STATUS_OK);
    assert(umi_update_channel_validate(&channel) == UMI_STATUS_OK);
    assert(umi_update_channel_offers(&channel, UMI_RELEASE_STABLE, 24U));
    assert(!umi_update_channel_offers(&channel, UMI_RELEASE_STABLE, 25U));
    assert(umi_upgrade_plan_init(
               &upgrade, "0.22.0", "0.23.0", 58U, 59U, 1) == UMI_STATUS_OK);
    assert(umi_upgrade_plan_authorise(&upgrade, 0) == UMI_STATUS_UNAVAILABLE);
    assert(umi_upgrade_plan_authorise(&upgrade, 1) == UMI_STATUS_OK);
    assert(umi_upgrade_plan_validate(&upgrade) == UMI_STATUS_OK);
    assert(umi_upgrade_plan_rollback_generation(&upgrade) == 58U);
    return 0;
}
