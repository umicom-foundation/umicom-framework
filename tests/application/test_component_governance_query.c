/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_component_governance_query.c
 *
 * PURPOSE:
 *   Verify allocation-free component discovery across product domain, search,
 *   role, maturity, lifecycle, frontend and evidence filters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/application.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiComponentInventory inventory;
  UmiComponentQuery query;
  UmiComponentQueryResult result;
  const UmiComponentGovernanceRecord *record;

  assert(umi_component_inventory_build(NULL, 0U, &inventory) == UMI_STATUS_OK);
  umi_component_query_init(&query);
  query.domain_id = "trading";
  assert(umi_component_inventory_query(&inventory, &query, &result) == UMI_STATUS_OK);
  /* Query coverage follows the live catalogue, including newly introduced
   * trading panels, instead of copying a count into this test. */
  assert(result.count == umi_application_component_domain_count("trading"));

  query.domain_id = NULL;
  query.search_text = "CHART";
  assert(umi_component_inventory_query(&inventory, &query, &result) == UMI_STATUS_OK);
  assert(result.count >= 1U);
  record = umi_component_query_result_at(&inventory, &result, 0U);
  assert(record != NULL);

  umi_component_query_init(&query);
  query.role_mask = umi_component_role_mask(UMI_APPLICATION_COMPONENT_EDITOR);
  query.status_mask = umi_component_api_status_mask(UMI_COMPONENT_API_PLANNED);
  assert(umi_component_inventory_query(&inventory, &query, &result) == UMI_STATUS_OK);
  assert(result.count > 0U);

  umi_component_query_init(&query);
  query.required_frontends = UMI_COMPONENT_FRONTEND_GTK4;
  assert(umi_component_inventory_query(&inventory, &query, &result) == UMI_STATUS_OK);
  assert(result.count == 0U);
  query.required_frontends = 1U << 30;
  assert(umi_component_inventory_query(&inventory, &query, &result) == UMI_STATUS_INVALID_ARGUMENT);
  umi_component_query_init(&query);
  query.status_mask = 1U << 30;
  assert(umi_component_inventory_query(&inventory, &query, &result) == UMI_STATUS_INVALID_ARGUMENT);
  return 0;
}
