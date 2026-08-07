#ifndef UMICOM_RUNTIME_MODULE_REGISTRY_H
#define UMICOM_RUNTIME_MODULE_REGISTRY_H

#include <stddef.h>
#include "umicom/runtime/module.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiModuleRegistry UmiModuleRegistry;
UmiStatus umi_module_registry_create(UmiModuleRegistry **out_registry);
void umi_module_registry_destroy(UmiModuleRegistry *registry);
UmiStatus umi_module_registry_add(UmiModuleRegistry *registry,
                                  const UmiModuleDescriptor *descriptor);
const UmiModuleDescriptor *umi_module_registry_find(const UmiModuleRegistry *registry,
                                                    const char *module_id);
size_t umi_module_registry_count(const UmiModuleRegistry *registry);
const UmiModuleDescriptor *umi_module_registry_at(const UmiModuleRegistry *registry,
                                                  size_t index);

#ifdef __cplusplus
}
#endif

#endif
