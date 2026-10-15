/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_condition.c
 *
 * PURPOSE:
 *   Implement the test condition behavior for
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
 * File: tests/runtime_bootstrap/test_condition.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe one property, feature, capability, platform or environment condition.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/runtime/bootstrap/condition.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted condition value. */
static int UmiBootstrapConditionConstructionCases(void)
{
    UmiBootstrapCondition *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_condition_init(value, UMI_BOOTSTRAP_CONDITION_PROPERTY_EQUALS, "app.mode", "prod", false) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->key) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_condition_init(value, UMI_BOOTSTRAP_CONDITION_PROPERTY_EQUALS, oversized, "prod", false) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_condition_init(value, UMI_BOOTSTRAP_CONDITION_PROPERTY_EQUALS, value->key, "prod", false) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->key, "app.mode") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->expected) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_condition_init(value, UMI_BOOTSTRAP_CONDITION_PROPERTY_EQUALS, "app.mode", oversized, false) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_condition_init(value, UMI_BOOTSTRAP_CONDITION_PROPERTY_EQUALS, "app.mode", value->expected, false) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->expected, "prod") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiBootstrapConditionConstructionCases() != 0) return 1;


    UmiBootstrapCondition c;
    CHECK(umi_bootstrap_condition_init(&c,UMI_BOOTSTRAP_CONDITION_PROPERTY_EQUALS,"app.mode","prod",false)==UMI_STATUS_OK);
    CHECK(strcmp(c.key,"app.mode")==0);
    return 0;
}
