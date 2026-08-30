/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_audit_channels.c
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

/* BEGINNER NOTE:
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/audit.h"
#include "umicom/delivery/channel_registry.h"
int main(void) {
    UmiDeliveryAuditEvent event;
    UmiChannelRegistry registry;
    UmiReleaseChannelPolicy policy = umi_release_channel_policy(UMI_RELEASE_STABLE);
    assert(umi_delivery_audit_event_init(&event, "e", "user", UMI_DELIVERY_STAGE_PACKAGE, "created") == UMI_STATUS_OK);
    umi_channel_registry_init(&registry);
    assert(umi_channel_registry_add(&registry, "stable", policy) == UMI_STATUS_OK);
    assert(umi_channel_registry_find(&registry, "stable") != 0);
    return 0;
}
