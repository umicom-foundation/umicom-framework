/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/factory.h
 *
 * PURPOSE:
 *   Define toolkit-adapter factories and a built-in headless implementation so
 *   the same component contract can be instantiated by multiple frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_FACTORY_H
#define UMICOM_APPLICATION_COMPONENT_FACTORY_H

#include "umicom/application/component/registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiApplicationComponentFactoryCreate)(
    const UmiApplicationComponentContract *contract, const char *instance_id,
    void *user_data, void **out_handle);
typedef UmiStatus (*UmiApplicationComponentFactoryLifecycle)(void *handle,
                                                             void *user_data);
typedef void (*UmiApplicationComponentFactoryDestroy)(void *handle,
                                                       void *user_data);

/**
 * Represent the application component factory descriptor data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationComponentFactoryDescriptor {
  uint32_t struct_size;
  uint32_t api_version;
  const char *factory_id;
  UmiApplicationComponentFrontend frontend;
  UmiApplicationComponentFactoryCreate create;
  UmiApplicationComponentFactoryLifecycle activate;
  UmiApplicationComponentFactoryLifecycle suspend;
  UmiApplicationComponentFactoryDestroy destroy;
  void *user_data;
} UmiApplicationComponentFactoryDescriptor;

/**
 * Represent the application component factory registry data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationComponentFactoryRegistry {
  UmiApplicationComponentFactoryDescriptor
      items[UMI_APPLICATION_COMPONENT_FACTORY_CAPACITY];
  size_t count;
  uint64_t revision;
} UmiApplicationComponentFactoryRegistry;

/**
 * Represent the application component headless handle data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationComponentHeadlessHandle {
  char instance_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char component_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  UmiApplicationComponentLifecycle lifecycle;
  uint64_t revision;
} UmiApplicationComponentHeadlessHandle;

/**
 * Initialise application component factory registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_component_factory_registry_init(
    UmiApplicationComponentFactoryRegistry *registry);
/**
 * Add application component factory registry only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_application_component_factory_registry_register(
    UmiApplicationComponentFactoryRegistry *registry,
    const UmiApplicationComponentFactoryDescriptor *descriptor);
/**
 * Provide the application component factory registry seed headless operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_component_factory_registry_seed_headless(
    UmiApplicationComponentFactoryRegistry *registry);
/**
 * Find application component factory registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationComponentFactoryDescriptor *
umi_application_component_factory_registry_find(
    const UmiApplicationComponentFactoryRegistry *registry,
    const char *factory_id, UmiApplicationComponentFrontend frontend);
/**
 * Return the number of records represented by application component factory registry
 * without changing their state.
 */
size_t umi_application_component_factory_registry_count(
    const UmiApplicationComponentFactoryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
