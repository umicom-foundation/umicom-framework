/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/runtime/module_registry.h
 *
 * PURPOSE:
 *   Declare the module registry contract shared by Framework services and thin
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RUNTIME_MODULE_REGISTRY_H
#define UMICOM_RUNTIME_MODULE_REGISTRY_H

#include <stddef.h>
#include "umicom/runtime/module.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the module registry data shared with callers of this public contract.
 */
typedef struct UmiModuleRegistry UmiModuleRegistry;
/**
 * Initialise module registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_module_registry_create(UmiModuleRegistry **out_registry);
/**
 * Release or reset state held by module registry so the same storage can be reused safely.
 */
void umi_module_registry_destroy(UmiModuleRegistry *registry);
/**
 * Add module registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_module_registry_add(UmiModuleRegistry *registry,
                                  const UmiModuleDescriptor *descriptor);
/**
 * Find module registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiModuleDescriptor *umi_module_registry_find(const UmiModuleRegistry *registry,
                                                    const char *module_id);
/**
 * Return the number of records represented by module registry without changing their
 * state.
 */
size_t umi_module_registry_count(const UmiModuleRegistry *registry);
/**
 * Find module registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiModuleDescriptor *umi_module_registry_at(const UmiModuleRegistry *registry,
                                                  size_t index);

#ifdef __cplusplus
}
#endif

#endif
