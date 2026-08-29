/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/factory.c
 *
 * PURPOSE:
 *   Register frontend factories and provide a deterministic headless adapter
 *   used by tests, automation, servers and non-graphical composition tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/factory.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/base/text.h"

static UmiStatus headless_create(
    const UmiApplicationComponentContract *contract, const char *instance_id,
    void *user_data, void **out_handle) {
  UmiApplicationComponentHeadlessHandle *handle;
  UmiStatus status;
  (void)user_data;
  if (contract == NULL || instance_id == NULL || out_handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out_handle = NULL;
  handle = (UmiApplicationComponentHeadlessHandle *)calloc(1U, sizeof(*handle));
  if (handle == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_text_copy(handle->instance_id, sizeof(handle->instance_id),
                         instance_id);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(handle->component_id, sizeof(handle->component_id),
                           contract->definition->component_id);
  if (status != UMI_STATUS_OK) {
    free(handle);
    return status;
  }
  handle->lifecycle = UMI_APPLICATION_COMPONENT_CREATED;
  handle->revision = 1U;
  *out_handle = handle;
  return UMI_STATUS_OK;
}

static UmiStatus headless_activate(void *value, void *user_data) {
  UmiApplicationComponentHeadlessHandle *handle =
      (UmiApplicationComponentHeadlessHandle *)value;
  (void)user_data;
  if (handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (handle->lifecycle != UMI_APPLICATION_COMPONENT_CREATED &&
      handle->lifecycle != UMI_APPLICATION_COMPONENT_SUSPENDED)
    return UMI_STATUS_INVALID_STATE;
  handle->lifecycle = UMI_APPLICATION_COMPONENT_ACTIVE;
  handle->revision += 1U;
  return UMI_STATUS_OK;
}

static UmiStatus headless_suspend(void *value, void *user_data) {
  UmiApplicationComponentHeadlessHandle *handle =
      (UmiApplicationComponentHeadlessHandle *)value;
  (void)user_data;
  if (handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (handle->lifecycle != UMI_APPLICATION_COMPONENT_ACTIVE)
    return UMI_STATUS_INVALID_STATE;
  handle->lifecycle = UMI_APPLICATION_COMPONENT_SUSPENDED;
  handle->revision += 1U;
  return UMI_STATUS_OK;
}

static void headless_destroy(void *value, void *user_data) {
  (void)user_data;
  free(value);
}

void umi_application_component_factory_registry_init(
    UmiApplicationComponentFactoryRegistry *registry) {
  if (registry != NULL) {
    (void)memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
  }
}

const UmiApplicationComponentFactoryDescriptor *
umi_application_component_factory_registry_find(
    const UmiApplicationComponentFactoryRegistry *registry,
    const char *factory_id, UmiApplicationComponentFrontend frontend) {
  size_t index;
  if (registry == NULL || factory_id == NULL ||
      registry->count > UMI_APPLICATION_COMPONENT_FACTORY_CAPACITY ||
      !umi_application_component_frontend_valid(frontend))
    return NULL;
  for (index = 0U; index < registry->count; ++index) {
    if (registry->items[index].frontend == frontend &&
        strcmp(registry->items[index].factory_id, factory_id) == 0)
      return &registry->items[index];
  }
  return NULL;
}

UmiStatus umi_application_component_factory_registry_register(
    UmiApplicationComponentFactoryRegistry *registry,
    const UmiApplicationComponentFactoryDescriptor *descriptor) {
  if (registry == NULL || descriptor == NULL || descriptor->factory_id == NULL ||
      descriptor->create == NULL || descriptor->destroy == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (registry->count > UMI_APPLICATION_COMPONENT_FACTORY_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  if (descriptor->struct_size < sizeof(*descriptor) ||
      descriptor->api_version != UMI_APPLICATION_COMPONENT_MODEL_API_VERSION ||
      descriptor->factory_id[0] == '\0' ||
      !umi_application_component_frontend_valid(descriptor->frontend))
    return UMI_STATUS_INVALID_STATE;
  if (umi_application_component_factory_registry_find(
          registry, descriptor->factory_id, descriptor->frontend) != NULL)
    return UMI_STATUS_ALREADY_EXISTS;
  if (registry->count >= UMI_APPLICATION_COMPONENT_FACTORY_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  registry->items[registry->count++] = *descriptor;
  registry->revision += 1U;
  return UMI_STATUS_OK;
}

UmiStatus umi_application_component_factory_registry_seed_headless(
    UmiApplicationComponentFactoryRegistry *registry) {
  static const char *const FACTORY_IDS[] = {
      "umicom.application.factory.view",
      "umicom.application.factory.editor",
      "umicom.application.factory.tool",
      "umicom.application.factory.dashboard",
      "umicom.application.factory.inspector",
      "umicom.application.factory.service-surface"};
  size_t index;
  if (registry == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (index = 0U; index < sizeof(FACTORY_IDS) / sizeof(FACTORY_IDS[0]); ++index) {
    UmiApplicationComponentFactoryDescriptor descriptor;
    UmiStatus status;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_APPLICATION_COMPONENT_MODEL_API_VERSION;
    descriptor.factory_id = FACTORY_IDS[index];
    descriptor.frontend = UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS;
    descriptor.create = headless_create;
    descriptor.activate = headless_activate;
    descriptor.suspend = headless_suspend;
    descriptor.destroy = headless_destroy;
    status = umi_application_component_factory_registry_register(registry,
                                                                 &descriptor);
    if (status != UMI_STATUS_OK)
      return status;
  }
  return UMI_STATUS_OK;
}

size_t umi_application_component_factory_registry_count(
    const UmiApplicationComponentFactoryRegistry *registry) {
  return registry != NULL &&
                 registry->count <= UMI_APPLICATION_COMPONENT_FACTORY_CAPACITY
             ? registry->count
             : 0U;
}
