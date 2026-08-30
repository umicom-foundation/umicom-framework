/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_environment_overlay.c
 *
 * PURPOSE:
 *   Verify task/launch environment assignments are parsed and overlaid without
 *   mutating the base environment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/toolchain/environment.h"

int main(void)
{
    UmiEnvironmentPlan base;
    UmiEnvironmentPlan overlay;
    UmiEnvironmentPlan combined;
    char name[128];
    char value[256];

    umi_environment_plan_init(&base);
    umi_environment_plan_init(&overlay);

    assert(umi_environment_plan_add(
        &base, "MODE", "base") == UMI_STATUS_OK);
    assert(umi_environment_plan_add(
        &base, "KEEP", "yes") == UMI_STATUS_OK);

    assert(umi_environment_plan_set_assignment(
        &overlay, "MODE=task") == UMI_STATUS_OK);
    assert(umi_environment_plan_set_assignment(
        &overlay, "NEW_VALUE=42") == UMI_STATUS_OK);

    assert(umi_environment_plan_compose(
        &base, &overlay, &combined) == UMI_STATUS_OK);

    assert(strcmp(umi_environment_plan_find(&base, "MODE"), "base") == 0);
    assert(strcmp(umi_environment_plan_find(&combined, "MODE"), "task") == 0);
    assert(strcmp(umi_environment_plan_find(&combined, "KEEP"), "yes") == 0);
    assert(strcmp(umi_environment_plan_find(
        &combined, "NEW_VALUE"), "42") == 0);

    assert(umi_environment_assignment_parse(
        "_PORTABLE_NAME=value=with=equals",
        name,
        sizeof(name),
        value,
        sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(name, "_PORTABLE_NAME") == 0);
    assert(strcmp(value, "value=with=equals") == 0);

    assert(umi_environment_plan_set_assignment(
        &overlay, "1INVALID=value") == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_environment_plan_set_assignment(
        &overlay, "MISSING_EQUALS") == UMI_STATUS_PARSE_ERROR);

    return 0;
}
