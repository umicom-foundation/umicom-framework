/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_toolchain_environment_composition.c
 *
 * PURPOSE:
 *   Implement the test toolchain environment composition behavior for
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
 * Umicom Framework - Batch 35 safe environment composition regression test.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "umicom/toolchain/environment.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEnvironmentPlan base;
    UmiEnvironmentPlan overlay;
    UmiEnvironmentPlan composed;
    umi_environment_plan_init(&base);
    umi_environment_plan_init(&overlay);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_environment_plan_add(&base, "PATH", "/usr/bin") != UMI_STATUS_OK ||
        umi_environment_plan_add(&base, "CC", "/usr/bin/cc") != UMI_STATUS_OK ||
        umi_environment_plan_add(&overlay, "CC", "/opt/gcc/bin/gcc") !=
            UMI_STATUS_OK ||
        umi_environment_plan_add(&overlay, "CMAKE_PREFIX_PATH", "/opt/sdk") !=
            UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_environment_plan_compose(&base, &overlay, &composed) !=
        UMI_STATUS_OK) return 2;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_environment_plan_find(&composed, "CC"),
               "/opt/gcc/bin/gcc") != 0 || composed.count != 3U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_environment_plan_append_path(&composed, "/opt/gcc/bin", 1) !=
            UMI_STATUS_OK ||
        strstr(umi_environment_plan_find(&composed, "PATH"),
               "/opt/gcc/bin") != umi_environment_plan_find(&composed, "PATH"))
        return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_environment_plan_validate(&composed) != UMI_STATUS_OK) return 5;
    return EXIT_SUCCESS;
}
