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
#include <string.h>

#include "umicom/application/application.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiComponentInventory inventory;
  UmiComponentGovernanceOverride overrides[2] = {{0}, {0}};
  const UmiComponentDomainInventory *trading;
  const UmiComponentGovernanceRecord *chart;

  assert(umi_component_inventory_build(NULL, 0U, &inventory) == UMI_STATUS_OK);
  assert(inventory.component_count == umi_application_component_catalogue_count());
  assert(inventory.domain_count == 31U);
  assert(umi_component_inventory_validate(&inventory) == UMI_STATUS_OK);
  trading = umi_component_inventory_domain_find(&inventory, "trading");
  assert(trading != NULL);
  assert(trading->component_count ==
         umi_application_component_domain_count("trading"));
  chart = umi_component_inventory_find(&inventory, "umicom.trading.chart");
  assert(chart != NULL);
  assert(chart->api_status == UMI_COMPONENT_API_CANDIDATE);

  overrides[0].component_id = "umicom.trading.chart";
  overrides[0].api_status = UMI_COMPONENT_API_STABLE;
  overrides[0].available_evidence = umi_component_evidence_known_mask();
  overrides[0].frontend_support = UMI_COMPONENT_FRONTEND_HEADLESS | UMI_COMPONENT_FRONTEND_GTK4;
  overrides[0].introduced_version = "1.0.0";
  overrides[0].replace_available_evidence = 1;
  overrides[0].replace_frontend_support = 1;
  assert(umi_component_inventory_build(overrides, 1U, &inventory) == UMI_STATUS_OK);
  chart = umi_component_inventory_find(&inventory, "umicom.trading.chart");
  assert(chart != NULL && chart->api_status == UMI_COMPONENT_API_STABLE);
  assert(umi_component_evidence_is_complete(chart));
  assert(strcmp(chart->introduced_version, "1.0.0") == 0);

  overrides[1] = overrides[0];
  assert(umi_component_inventory_build(overrides, 2U, &inventory) == UMI_STATUS_ALREADY_EXISTS);
  overrides[0].component_id = "umicom.unknown.component";
  assert(umi_component_inventory_build(overrides, 1U, &inventory) == UMI_STATUS_NOT_FOUND);
  return 0;
}
