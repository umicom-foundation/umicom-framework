/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_component/test_contract_registry.c
 *
 * PURPOSE:
 *   Verify every current catalogue record becomes a discoverable, versioned
 *   component contract through the single Framework registry.
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiApplicationComponentRegistry registry;
  const UmiApplicationComponentContract *contract;
  size_t catalogue_count = umi_application_component_catalogue_count();
  UmiApplicationComponentVersion required = {1U, 0U, 0U};
  UmiApplicationComponentVersion future = {1U, 1U, 0U};

  umi_application_component_registry_init(&registry);
  assert(umi_application_component_registry_seed_catalogue(&registry) ==
         UMI_STATUS_OK);
  /* The catalogue is authoritative. This assertion continues to protect full
   * registry coverage when a new reusable component is added later. */
  assert(umi_application_component_registry_count(&registry) == catalogue_count);
  assert(umi_application_component_registry_revision(&registry) ==
         (uint64_t)catalogue_count + 1U);
  contract = umi_application_component_registry_find(
      &registry, "umicom.trading.chart");
  assert(contract != NULL);
  assert(strcmp(contract->factory_id,
                "umicom.application.factory.editor") == 0);
  assert(umi_application_component_contract_supports(
      contract, UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS));
  assert(umi_application_component_contract_supports(
      contract, UMI_APPLICATION_COMPONENT_FRONTEND_GTK4));
  assert(!umi_application_component_contract_supports(
      contract, (UmiApplicationComponentFrontend)(
                    UMI_APPLICATION_COMPONENT_FRONTEND_GTK4 |
                    UMI_APPLICATION_COMPONENT_FRONTEND_WEB)));
  assert(umi_application_component_version_compatible(contract->version,
                                                      required));
  assert(!umi_application_component_version_compatible(contract->version,
                                                       future));
  assert(umi_application_component_registry_find(&registry, "unknown") == NULL);
  registry.count = UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY + 1U;
  assert(umi_application_component_registry_count(&registry) == 0U);
  assert(umi_application_component_registry_find(
             &registry, "umicom.trading.chart") == NULL);
  return 0;
}
