/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_condition_evaluator.c
 *
 * PURPOSE:
 *   Implement the test condition evaluator behavior for
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
 * File: tests/runtime_bootstrap/test_condition_evaluator.c
 *
 * PURPOSE:
 *   Focused regression coverage for Evaluate auto-configuration conditions against Framework bootstrap context.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/runtime/bootstrap/configuration_property.h"
#include "umicom/runtime/bootstrap/configuration_overlay.h"
#include "umicom/runtime/bootstrap/condition.h"
#include "umicom/runtime/bootstrap/condition_set.h"
#include "umicom/runtime/bootstrap/condition_evaluator.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted configuration property value. */
static int UmiBootstrapConfigurationPropertyConstructionCases(void)
{
    UmiBootstrapConfigurationProperty *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_configuration_property_init(value, "app.mode", "prod", 1, false) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->key) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_configuration_property_init(value, oversized, "prod", 1, false) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_configuration_property_init(value, value->key, "prod", 1, false) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->key, "app.mode") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->value) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_configuration_property_init(value, "app.mode", oversized, 1, false) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_configuration_property_init(value, "app.mode", value->value, 1, false) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->value, "prod") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiBootstrapConfigurationPropertyConstructionCases() != 0) return 1;


    UmiBootstrapPropertySet props,env; UmiBootstrapCondition c; UmiBootstrapConditionSet set;
    UmiBootstrapIdList features={0},caps={0};
    umi_bootstrap_property_set_init(&props); umi_bootstrap_property_set_init(&env);
    { UmiBootstrapConfigurationProperty p; CHECK(umi_bootstrap_configuration_property_init(&p,"app.mode","prod",1,false)==UMI_STATUS_OK); CHECK(umi_bootstrap_configuration_overlay_apply(&props,&p)==UMI_STATUS_OK); }
    CHECK(umi_bootstrap_condition_init(&c,UMI_BOOTSTRAP_CONDITION_PROPERTY_EQUALS,"app.mode","prod",false)==UMI_STATUS_OK);
    umi_bootstrap_condition_set_init(&set,UMI_BOOTSTRAP_CONDITION_ALL); CHECK(umi_bootstrap_condition_set_add(&set,&c)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_condition_set_evaluate(&set,&props,&env,&features,&caps,"windows"));
    return 0;
}
