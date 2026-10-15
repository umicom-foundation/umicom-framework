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
#include "value_archive/construction_cases.h"
#include "umicom/delivery/deployment.h"
#include "umicom/delivery/health.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted deployment value. */
static int UmiDeploymentConstructionCases(void)
{
    UmiDeployment *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_deployment_init(value, "d1", "r1", "local", 2U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->deployment_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_deployment_init(value, oversized, "r1", "local", 2U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_deployment_init(value, value->deployment_id, "r1", "local", 2U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->deployment_id, "d1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->release_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_deployment_init(value, "d1", oversized, "local", 2U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_deployment_init(value, "d1", value->release_id, "local", 2U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->release_id, "r1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->target) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_deployment_init(value, "d1", "r1", oversized, 2U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_deployment_init(value, "d1", "r1", value->target, 2U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->target, "local") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDeploymentConstructionCases() != 0) return 1;

    UmiDeployment deployment;
    UmiDeliveryHealth health;
    assert(umi_deployment_init(&deployment, "d1", "r1", "local", 2U) == UMI_STATUS_OK);
    umi_delivery_health_init(&health);
    health.ready = 1; health.live = 1; health.checks_passed = 2U;
    assert(umi_delivery_health_acceptable(&health));
    return 0;
}
