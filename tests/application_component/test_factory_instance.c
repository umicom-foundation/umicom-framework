/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_component/test_factory_instance.c
 *
 * PURPOSE:
 *   Verify headless factory discovery and the complete create, activate,
 *   suspend and close lifecycle for a reusable component instance.
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
#include <string.h>

int main(void) {
  UmiApplicationComponentRegistry components;
  UmiApplicationComponentFactoryRegistry factories;
  UmiApplicationComponentInstanceRequest request = {
      "umicom.development.editor", "studio.editor.main",
      UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS};
  UmiApplicationComponentInstance instance;
  UmiApplicationComponentHeadlessHandle *handle;

  umi_application_component_registry_init(&components);
  umi_application_component_factory_registry_init(&factories);
  assert(umi_application_component_registry_seed_catalogue(&components) ==
         UMI_STATUS_OK);
  assert(umi_application_component_factory_registry_seed_headless(&factories) ==
         UMI_STATUS_OK);
  assert(umi_application_component_factory_registry_count(&factories) == 6U);
  assert(umi_application_component_instance_create(
             &components, &factories, &request, &instance) == UMI_STATUS_OK);
  assert(instance.lifecycle == UMI_APPLICATION_COMPONENT_CREATED);
  handle = (UmiApplicationComponentHeadlessHandle *)instance.handle;
  assert(strcmp(handle->component_id, request.component_id) == 0);
  assert(umi_application_component_instance_activate(&instance) == UMI_STATUS_OK);
  assert(handle->lifecycle == UMI_APPLICATION_COMPONENT_ACTIVE);
  assert(umi_application_component_instance_suspend(&instance) == UMI_STATUS_OK);
  assert(handle->lifecycle == UMI_APPLICATION_COMPONENT_SUSPENDED);
  assert(umi_application_component_instance_activate(&instance) == UMI_STATUS_OK);
  assert(umi_application_component_instance_close(&instance) == UMI_STATUS_OK);
  assert(instance.lifecycle == UMI_APPLICATION_COMPONENT_CLOSED);
  assert(instance.handle == NULL);
  assert(umi_application_component_instance_close(&instance) == UMI_STATUS_OK);
  return 0;
}
