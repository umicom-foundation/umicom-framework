/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_auto_configuration.c
 *
 * PURPOSE:
 *   Implement the test auto configuration behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_auto_configuration.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe conditional Framework-owned auto-configuration contributions.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/runtime/bootstrap/auto_configuration.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted auto configuration value. */
static int UmiBootstrapAutoConfigurationConstructionCases(void)
{
    UmiBootstrapAutoConfiguration *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_auto_configuration_init(value, "autoconfig.data", 10, UMI_BOOTSTRAP_CONDITION_ALL) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->configuration_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_auto_configuration_init(value, oversized, 10, UMI_BOOTSTRAP_CONDITION_ALL) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_auto_configuration_init(value, value->configuration_id, 10, UMI_BOOTSTRAP_CONDITION_ALL) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->configuration_id, "autoconfig.data") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiBootstrapAutoConfigurationConstructionCases() != 0) return 1;


    UmiBootstrapAutoConfiguration c;
    CHECK(umi_bootstrap_auto_configuration_init(&c,"autoconfig.data",10,UMI_BOOTSTRAP_CONDITION_ALL)==UMI_STATUS_OK);
    CHECK(c.enabled && c.priority==10);
    return 0;
}
