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

size_t umi_developer_workbench_builtin_perspective_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_developer_workbench_builtin_perspective_at(size_t index)
{
    return index < umi_developer_workbench_builtin_perspective_count()
        ? FACTORIES[index]()
        : NULL;
}

UmiStatus umi_developer_workbench_builtin_perspectives_register(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry)
{
    size_t index;

    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    for (index = 0U;
         index < umi_developer_workbench_builtin_perspective_count();
         ++index) {
        const UmiStatus status =
            umi_developer_workbench_perspective_registry_register(
                registry,
                FACTORIES[index]());

        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
