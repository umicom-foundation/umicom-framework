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

typedef struct UmiApplicationComponentFactoryRegistry {
  UmiApplicationComponentFactoryDescriptor
      items[UMI_APPLICATION_COMPONENT_FACTORY_CAPACITY];
  size_t count;
  uint64_t revision;
} UmiApplicationComponentFactoryRegistry;

typedef struct UmiApplicationComponentHeadlessHandle {
  char instance_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char component_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  UmiApplicationComponentLifecycle lifecycle;
  uint64_t revision;
} UmiApplicationComponentHeadlessHandle;

void umi_application_component_factory_registry_init(
    UmiApplicationComponentFactoryRegistry *registry);
UmiStatus umi_application_component_factory_registry_register(
    UmiApplicationComponentFactoryRegistry *registry,
    const UmiApplicationComponentFactoryDescriptor *descriptor);
UmiStatus umi_application_component_factory_registry_seed_headless(
    UmiApplicationComponentFactoryRegistry *registry);
const UmiApplicationComponentFactoryDescriptor *
umi_application_component_factory_registry_find(
    const UmiApplicationComponentFactoryRegistry *registry,
    const char *factory_id, UmiApplicationComponentFrontend frontend);
size_t umi_application_component_factory_registry_count(
    const UmiApplicationComponentFactoryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
