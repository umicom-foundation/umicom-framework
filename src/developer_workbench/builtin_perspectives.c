/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/builtin_perspectives.c
 *
 * PURPOSE:
 *   Enumerate and install the professional built-in developer perspectives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/builtin_perspectives.h"

#include "umicom/developer_workbench/perspectives/perspectives.h"

typedef const UmiDeveloperWorkbenchPerspectiveDefinition *
    (*PerspectiveFactory)(void);

static const PerspectiveFactory FACTORIES[] = {
    umi_developer_workbench_perspective_coding,
    umi_developer_workbench_perspective_debugging,
    umi_developer_workbench_perspective_testing,
    umi_developer_workbench_perspective_source_control,
    umi_developer_workbench_perspective_design,
    umi_developer_workbench_perspective_ai,
    umi_developer_workbench_perspective_delivery
};

/*
 * Return the number of records represented by developer workbench builtin perspective
 * without changing their state.
 */
size_t umi_developer_workbench_builtin_perspective_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

/*
 * Find developer workbench builtin perspective while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_developer_workbench_builtin_perspective_at(size_t index)
{
    return index < umi_developer_workbench_builtin_perspective_count()
        ? FACTORIES[index]()
        : NULL;
}

/*
 * Add developer workbench builtin perspectives only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_developer_workbench_builtin_perspectives_register(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_developer_workbench_builtin_perspective_count();
         ++index) {
        const UmiStatus status =
            umi_developer_workbench_perspective_registry_register(
                registry,
                FACTORIES[index]());

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
