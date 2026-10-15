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

/*
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "value_archive/construction_cases.h"
#include "umicom/delivery/audit.h"
#include "umicom/delivery/channel_registry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted audit value. */
static int UmiDeliveryAuditEventConstructionCases(void)
{
    UmiDeliveryAuditEvent *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_delivery_audit_event_init(value, "e", "user", UMI_DELIVERY_STAGE_PACKAGE, "created") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->event_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_audit_event_init(value, oversized, "user", UMI_DELIVERY_STAGE_PACKAGE, "created") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_audit_event_init(value, value->event_id, "user", UMI_DELIVERY_STAGE_PACKAGE, "created") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->event_id, "e") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->actor_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_audit_event_init(value, "e", oversized, UMI_DELIVERY_STAGE_PACKAGE, "created") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_audit_event_init(value, "e", value->actor_id, UMI_DELIVERY_STAGE_PACKAGE, "created") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->actor_id, "user") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->message) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_audit_event_init(value, "e", "user", UMI_DELIVERY_STAGE_PACKAGE, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_audit_event_init(value, "e", "user", UMI_DELIVERY_STAGE_PACKAGE, value->message) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->message, "created") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDeliveryAuditEventConstructionCases() != 0) return 1;

    UmiDeliveryAuditEvent event;
    UmiChannelRegistry registry;
    UmiReleaseChannelPolicy policy = umi_release_channel_policy(UMI_RELEASE_STABLE);
    assert(umi_delivery_audit_event_init(&event, "e", "user", UMI_DELIVERY_STAGE_PACKAGE, "created") == UMI_STATUS_OK);
    umi_channel_registry_init(&registry);
    assert(umi_channel_registry_add(&registry, "stable", policy) == UMI_STATUS_OK);
    assert(umi_channel_registry_find(&registry, "stable") != 0);
    return 0;
}
