/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/module_registry.c
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

UmiStatus umi_module_registry_create(UmiModuleRegistry **out_registry)
{
    if (out_registry == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = calloc(1U, sizeof(UmiModuleRegistry));
    return *out_registry != 0 ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}
void umi_module_registry_destroy(UmiModuleRegistry *registry) { free(registry); }
UmiStatus umi_module_registry_add(UmiModuleRegistry *registry,
                                  const UmiModuleDescriptor *descriptor)
{
    size_t i;
    if (registry == 0 || descriptor == 0 || descriptor->module_id == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (descriptor->structure_size != sizeof(UmiModuleDescriptor) ||
        descriptor->abi_version != UMICOM_FRAMEWORK_ABI_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < registry->count; ++i)
        if (strcmp(registry->items[i]->module_id, descriptor->module_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count >= UMI_MODULE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = descriptor;
    return UMI_STATUS_OK;
}
const UmiModuleDescriptor *umi_module_registry_find(const UmiModuleRegistry *registry,
                                                    const char *module_id)
{
    size_t i;
    if (registry == 0 || module_id == 0) return 0;
    for (i = 0U; i < registry->count; ++i)
        if (strcmp(registry->items[i]->module_id, module_id) == 0)
            return registry->items[i];
    return 0;
}
size_t umi_module_registry_count(const UmiModuleRegistry *registry)
{
    return registry != 0 ? registry->count : 0U;
}
const UmiModuleDescriptor *umi_module_registry_at(const UmiModuleRegistry *registry,
                                                  size_t index)
{
    return registry != 0 && index < registry->count ? registry->items[index] : 0;
}
