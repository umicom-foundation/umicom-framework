/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_inventory_environment.c
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
#include "umicom/delivery/inventory.h"
#include "umicom/delivery/environment.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted environment value. */
static int UmiDeliveryEnvironmentConstructionCases(void)
{
    UmiDeliveryEnvironment *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_delivery_environment_init(value, "windows", "x86_64", "release") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->operating_system) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_environment_init(value, oversized, "x86_64", "release") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_environment_init(value, value->operating_system, "x86_64", "release") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->operating_system, "windows") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->architecture) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_environment_init(value, "windows", oversized, "release") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_environment_init(value, "windows", value->architecture, "release") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->architecture, "x86_64") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->configuration) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_environment_init(value, "windows", "x86_64", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_environment_init(value, "windows", "x86_64", value->configuration) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->configuration, "release") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDeliveryEnvironmentConstructionCases() != 0) return 1;

    UmiDeliveryInventory inventory;
    UmiDeliveryEnvironment environment;
    umi_delivery_inventory_init(&inventory);
    assert(umi_delivery_inventory_add(&inventory, "bin/app", 10U) == UMI_STATUS_OK);
    assert(umi_delivery_inventory_total_bytes(&inventory) == 10U);
    assert(umi_delivery_environment_init(&environment, "windows", "x86_64", "release") == UMI_STATUS_OK);
    return 0;
}
