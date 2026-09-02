/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/runtime/module_registry.c
 *
 * PURPOSE:
 *   Implement the module registry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/module_registry.h"

#include <stdlib.h>
#include <string.h>

#define UMI_MODULE_MAX 128U

struct UmiModuleRegistry {
    const UmiModuleDescriptor *items[UMI_MODULE_MAX];
    size_t count;
};

/*
 * Initialise module registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_module_registry_create(UmiModuleRegistry **out_registry)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_registry == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = calloc(1U, sizeof(UmiModuleRegistry));
    return *out_registry != 0 ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}
/* Release or reset state held by module registry so the same storage can be reused safely. */
void umi_module_registry_destroy(UmiModuleRegistry *registry) { free(registry); }
/* Add module registry only after its inputs and available capacity have been checked. */
UmiStatus umi_module_registry_add(UmiModuleRegistry *registry,
                                  const UmiModuleDescriptor *descriptor)
{
    size_t i;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (registry == 0 || descriptor == 0 || descriptor->module_id == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->structure_size != sizeof(UmiModuleDescriptor) ||
        descriptor->abi_version != UMICOM_FRAMEWORK_ABI_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[i]->module_id, descriptor->module_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_MODULE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = descriptor;
    return UMI_STATUS_OK;
}
/*
 * Find module registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiModuleDescriptor *umi_module_registry_find(const UmiModuleRegistry *registry,
                                                    const char *module_id)
{
    size_t i;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (registry == 0 || module_id == 0) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[i]->module_id, module_id) == 0)
            return registry->items[i];
    return 0;
}
/*
 * Return the number of records represented by module registry without changing their
 * state.
 */
size_t umi_module_registry_count(const UmiModuleRegistry *registry)
{
    return registry != 0 ? registry->count : 0U;
}
/*
 * Find module registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiModuleDescriptor *umi_module_registry_at(const UmiModuleRegistry *registry,
                                                  size_t index)
{
    return registry != 0 && index < registry->count ? registry->items[index] : 0;
}
