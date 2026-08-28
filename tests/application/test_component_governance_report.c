/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_component_governance_report.c
 *
 * PURPOSE:
 *   Verify two-pass report sizing, bounded output and deterministic text,
 *   Markdown and JSON inventories for all reusable Framework components.
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

static void verify_format(const UmiComponentInventory *inventory, UmiComponentReportFormat format,
                          const char *expected_text) {
  char small[32];
  char *output;
  size_t required = 0U;

  assert(umi_component_inventory_report_write(inventory, NULL, format, NULL, 0U, &required) ==
         UMI_STATUS_OK);
  assert(required > sizeof(small));
  assert(umi_component_inventory_report_write(inventory, NULL, format, small, sizeof(small),
                                              &required) == UMI_STATUS_CAPACITY_EXCEEDED);
  output = malloc(required);
  assert(output != NULL);
  assert(umi_component_inventory_report_write(inventory, NULL, format, output, required,
                                              &required) == UMI_STATUS_OK);
  assert(strstr(output, expected_text) != NULL);
  assert(strstr(output, "umicom.trading.chart") != NULL);
  free(output);
}

int main(void) {
  UmiComponentInventory inventory;
  UmiComponentQuery query;
  char output[8192];
  size_t required = 0U;

  assert(umi_component_inventory_build(NULL, 0U, &inventory) == UMI_STATUS_OK);
  verify_format(&inventory, UMI_COMPONENT_REPORT_TEXT, "components=132");
  verify_format(&inventory, UMI_COMPONENT_REPORT_MARKDOWN,
                "# Umicom Framework Component Inventory");
  verify_format(&inventory, UMI_COMPONENT_REPORT_JSON, "\"component_count\":132");

  umi_component_query_init(&query);
  query.domain_id = "trading";
  assert(umi_component_inventory_report_write(&inventory, &query, UMI_COMPONENT_REPORT_JSON, output,
                                              sizeof(output), &required) == UMI_STATUS_OK);
  assert(strstr(output, "\"selected_count\":9") != NULL);
  assert(strstr(output, "umicom.treasury") == NULL);
  return 0;
}
