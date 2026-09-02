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

/*
 * Initialise headless from caller-provided values so later operations receive a known
 * state.
 */
static UmiStatus headless_create(
    const UmiApplicationComponentContract *contract, const char *instance_id,
    void *user_data, void **out_handle) {
  UmiApplicationComponentHeadlessHandle *handle;
  UmiStatus status;
  (void)user_data;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (contract == NULL || instance_id == NULL || out_handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out_handle = NULL;
  handle = (UmiApplicationComponentHeadlessHandle *)calloc(1U, sizeof(*handle));
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (handle == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_text_copy(handle->instance_id, sizeof(handle->instance_id),
                         instance_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(handle->component_id, sizeof(handle->component_id),
                           contract->definition->component_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK) {
    free(handle);
    return status;
  }
  handle->lifecycle = UMI_APPLICATION_COMPONENT_CREATED;
  handle->revision = 1U;
  *out_handle = handle;
  return UMI_STATUS_OK;
}

/* Provide the headless activate operation used by this module and its client applications. */
static UmiStatus headless_activate(void *value, void *user_data) {
  UmiApplicationComponentHeadlessHandle *handle =
      (UmiApplicationComponentHeadlessHandle *)value;
  (void)user_data;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this branch only when its contract condition is satisfied. */
  if (handle->lifecycle != UMI_APPLICATION_COMPONENT_CREATED &&
      handle->lifecycle != UMI_APPLICATION_COMPONENT_SUSPENDED)
    return UMI_STATUS_INVALID_STATE;
  handle->lifecycle = UMI_APPLICATION_COMPONENT_ACTIVE;
  handle->revision += 1U;
  return UMI_STATUS_OK;
}

/* Provide the headless suspend operation used by this module and its client applications. */
static UmiStatus headless_suspend(void *value, void *user_data) {
  UmiApplicationComponentHeadlessHandle *handle =
      (UmiApplicationComponentHeadlessHandle *)value;
  (void)user_data;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this operation only while the related capability or state is available. */
  if (handle->lifecycle != UMI_APPLICATION_COMPONENT_ACTIVE)
    return UMI_STATUS_INVALID_STATE;
  handle->lifecycle = UMI_APPLICATION_COMPONENT_SUSPENDED;
  handle->revision += 1U;
  return UMI_STATUS_OK;
}

/* Release or reset state held by headless so the same storage can be reused safely. */
static void headless_destroy(void *value, void *user_data) {
  (void)user_data;
  free(value);
}

/*
 * Initialise application component factory registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_component_factory_registry_init(
    UmiApplicationComponentFactoryRegistry *registry) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry != NULL) {
    (void)memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
  }
}

/*
 * Find application component factory registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationComponentFactoryDescriptor *
umi_application_component_factory_registry_find(
    const UmiApplicationComponentFactoryRegistry *registry,
    const char *factory_id, UmiApplicationComponentFrontend frontend) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL || factory_id == NULL ||
      registry->count > UMI_APPLICATION_COMPONENT_FACTORY_CAPACITY ||
      !umi_application_component_frontend_valid(frontend))
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < registry->count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->items[index].frontend == frontend &&
        strcmp(registry->items[index].factory_id, factory_id) == 0)
      return &registry->items[index];
  }
  return NULL;
}

/*
 * Add application component factory registry only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_application_component_factory_registry_register(
    UmiApplicationComponentFactoryRegistry *registry,
    const UmiApplicationComponentFactoryDescriptor *descriptor) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL || descriptor == NULL || descriptor->factory_id == NULL ||
      descriptor->create == NULL || descriptor->destroy == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (registry->count > UMI_APPLICATION_COMPONENT_FACTORY_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  /* Apply this branch only when its contract condition is satisfied. */
  if (descriptor->struct_size < sizeof(*descriptor) ||
      descriptor->api_version != UMI_APPLICATION_COMPONENT_MODEL_API_VERSION ||
      descriptor->factory_id[0] == '\0' ||
      !umi_application_component_frontend_valid(descriptor->frontend))
    return UMI_STATUS_INVALID_STATE;
  /* Apply this branch only when its contract condition is satisfied. */
  if (umi_application_component_factory_registry_find(
          registry, descriptor->factory_id, descriptor->frontend) != NULL)
    return UMI_STATUS_ALREADY_EXISTS;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (registry->count >= UMI_APPLICATION_COMPONENT_FACTORY_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  registry->items[registry->count++] = *descriptor;
  registry->revision += 1U;
  return UMI_STATUS_OK;
}

/*
 * Provide the application component factory registry seed headless operation used by this
 * module and its client applications.
 */
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
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Visit each bounded item once so every record receives the same rule. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
      return status;
  }
  return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by application component factory registry
 * without changing their state.
 */
size_t umi_application_component_factory_registry_count(
    const UmiApplicationComponentFactoryRegistry *registry) {
  return registry != NULL &&
                 registry->count <= UMI_APPLICATION_COMPONENT_FACTORY_CAPACITY
             ? registry->count
             : 0U;
}
