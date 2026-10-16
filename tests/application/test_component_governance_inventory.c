/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_component_governance_inventory.c
 *
 * PURPOSE:
 *   Verify all catalogue components receive governance records, aggregate into
 *   deterministic domains and accept only unique, known explicit overrides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/application.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiComponentInventory *inventory =
      (UmiComponentInventory *)calloc(1U, sizeof(*inventory));
  UmiComponentGovernanceOverride overrides[2] = {{0}, {0}};
  const UmiComponentDomainInventory *trading;
  const UmiComponentGovernanceRecord *chart;

  /* The inventory contains every component and domain, so keep it on the
   * heap and leave the native stack available for the test harness. */
  assert(inventory != NULL);
  assert(umi_component_inventory_build(NULL, 0U, inventory) == UMI_STATUS_OK);
  assert(inventory->component_count == umi_application_component_catalogue_count());
/* The fixed domain total predates the canonical Healthcare component; validate complete domain membership instead. The former implementation is retained for review. */
#if 0
  assert(inventory->domain_count == 31U);
#endif
  /* New product domains must be governed too. Count distinct canonical domain
   * identities rather than freezing the inventory at an earlier portfolio. */
  size_t expected_domains = 0U;
  for (size_t index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    const UmiApplicationComponentDefinition *component =
        umi_application_component_catalogue_at(index);
    assert(component != NULL);
    size_t previous = 0U;
    for (; previous < index; ++previous) {
      const UmiApplicationComponentDefinition *earlier =
          umi_application_component_catalogue_at(previous);
      assert(earlier != NULL);
      if (strcmp(earlier->domain_id, component->domain_id) == 0) break;
    }
    if (previous == index) {
      const UmiComponentDomainInventory *domain =
          umi_component_inventory_domain_find(inventory, component->domain_id);
      assert(domain != NULL);
      assert(domain->component_count == umi_application_component_domain_count(component->domain_id));
      ++expected_domains;
    }
  }
  assert(inventory->domain_count == expected_domains);
  assert(umi_component_inventory_domain_find(inventory, "healthcare") != NULL);
  assert(umi_component_inventory_validate(inventory) == UMI_STATUS_OK);
  trading = umi_component_inventory_domain_find(inventory, "trading");
  assert(trading != NULL);
  assert(trading->component_count ==
         umi_application_component_domain_count("trading"));
  chart = umi_component_inventory_find(inventory, "umicom.trading.chart");
  assert(chart != NULL);
  assert(chart->api_status == UMI_COMPONENT_API_CANDIDATE);

  overrides[0].component_id = "umicom.trading.chart";
  overrides[0].api_status = UMI_COMPONENT_API_STABLE;
  overrides[0].available_evidence = umi_component_evidence_known_mask();
  overrides[0].frontend_support = UMI_COMPONENT_FRONTEND_HEADLESS | UMI_COMPONENT_FRONTEND_GTK4;
  overrides[0].introduced_version = "1.0.0";
  overrides[0].replace_available_evidence = 1;
  overrides[0].replace_frontend_support = 1;
  assert(umi_component_inventory_build(overrides, 1U, inventory) == UMI_STATUS_OK);
  chart = umi_component_inventory_find(inventory, "umicom.trading.chart");
  assert(chart != NULL && chart->api_status == UMI_COMPONENT_API_STABLE);
  assert(umi_component_evidence_is_complete(chart));
  assert(strcmp(chart->introduced_version, "1.0.0") == 0);

  overrides[1] = overrides[0];
  assert(umi_component_inventory_build(overrides, 2U, inventory) == UMI_STATUS_ALREADY_EXISTS);
  overrides[0].component_id = "umicom.unknown.component";
  assert(umi_component_inventory_build(overrides, 1U, inventory) == UMI_STATUS_NOT_FOUND);
  free(inventory);
  return 0;
}
