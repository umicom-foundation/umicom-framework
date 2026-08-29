/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_component/test_frontend_adapter.c
 *
 * PURPOSE:
 *   Verify an external GTK4-style adapter can register against the same
 *   versioned factory contract and instantiate a catalogue component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/component.h"

#include <assert.h>
#include <stdlib.h>

static UmiStatus adapter_create(
    const UmiApplicationComponentContract *contract, const char *instance_id,
    void *user_data, void **out_handle) {
  int *handle;
  (void)contract;
  (void)instance_id;
  (void)user_data;
  if (out_handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  handle = (int *)calloc(1U, sizeof(*handle));
  if (handle == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  *handle = 1;
  *out_handle = handle;
  return UMI_STATUS_OK;
}

static UmiStatus adapter_activate(void *handle, void *user_data) {
  (void)user_data;
  if (handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *(int *)handle = 2;
  return UMI_STATUS_OK;
}

static void adapter_destroy(void *handle, void *user_data) {
  (void)user_data;
  free(handle);
}

int main(void) {
  UmiApplicationComponentRegistry components;
  UmiApplicationComponentFactoryRegistry factories;
  UmiApplicationComponentFactoryDescriptor descriptor = {0};
  UmiApplicationComponentInstanceRequest request = {
      "umicom.trading.watchlist", "trader.watchlist.main",
      UMI_APPLICATION_COMPONENT_FRONTEND_GTK4};
  UmiApplicationComponentInstance instance;

  umi_application_component_registry_init(&components);
  umi_application_component_factory_registry_init(&factories);
  assert(umi_application_component_registry_seed_catalogue(&components) ==
         UMI_STATUS_OK);
  descriptor.struct_size = (uint32_t)sizeof(descriptor);
  descriptor.api_version = UMI_APPLICATION_COMPONENT_MODEL_API_VERSION;
  descriptor.factory_id = "umicom.application.factory.view";
  descriptor.frontend = UMI_APPLICATION_COMPONENT_FRONTEND_GTK4;
  descriptor.create = adapter_create;
  descriptor.activate = adapter_activate;
  descriptor.destroy = adapter_destroy;
  descriptor.frontend = (UmiApplicationComponentFrontend)(
      UMI_APPLICATION_COMPONENT_FRONTEND_GTK4 |
      UMI_APPLICATION_COMPONENT_FRONTEND_WEB);
  assert(umi_application_component_factory_registry_register(
             &factories, &descriptor) == UMI_STATUS_INVALID_STATE);
  descriptor.frontend = UMI_APPLICATION_COMPONENT_FRONTEND_GTK4;
  assert(umi_application_component_factory_registry_register(
             &factories, &descriptor) == UMI_STATUS_OK);
  assert(umi_application_component_instance_create(
             &components, &factories, &request, &instance) == UMI_STATUS_OK);
  assert(umi_application_component_instance_activate(&instance) == UMI_STATUS_OK);
  assert(*(int *)instance.handle == 2);
  assert(umi_application_component_instance_close(&instance) == UMI_STATUS_OK);
  return 0;
}
