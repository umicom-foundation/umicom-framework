/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_install.c
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
#include "umicom/delivery/install_plan.h"
#include "umicom/delivery/install_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted install state value. */
static int UmiInstallStateConstructionCases(void)
{
    UmiInstallState *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_install_state_init(value, "studio", "0.14.0", 4U, "/opt/studio") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->application_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_install_state_init(value, oversized, "0.14.0", 4U, "/opt/studio") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_install_state_init(value, value->application_id, "0.14.0", 4U, "/opt/studio") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->application_id, "studio") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->version) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_install_state_init(value, "studio", oversized, 4U, "/opt/studio") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_install_state_init(value, "studio", value->version, 4U, "/opt/studio") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->version, "0.14.0") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->install_root) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_install_state_init(value, "studio", "0.14.0", 4U, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_install_state_init(value, "studio", "0.14.0", 4U, value->install_root) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->install_root, "/opt/studio") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiInstallStateConstructionCases() != 0) return 1;

    UmiInstallPlan plan;
    UmiInstallState state;
    umi_install_plan_init(&plan);
    assert(umi_install_plan_add(&plan, UMI_INSTALL_COPY_FILE, "a", "b") == UMI_STATUS_OK);
    assert(plan.count == 1U);
    assert(umi_install_state_init(&state, "studio", "0.14.0", 4U, "/opt/studio") == UMI_STATUS_OK);
    assert(state.generation == 4U);
    return 0;
}
