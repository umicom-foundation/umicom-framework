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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/application.h"

/*
 * Exercise verify format and return a clear result when the behaviour no longer matches
 * its contract.
 */
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiComponentInventory inventory;
  UmiComponentQuery query;
  char output[8192];
  char expected[64];
  size_t required = 0U;

  assert(umi_component_inventory_build(NULL, 0U, &inventory) == UMI_STATUS_OK);
  /* Build expected totals from the catalogue-backed inventory. This keeps the
   * report test strict while allowing the Framework catalogue to grow. */
  assert(snprintf(expected, sizeof(expected), "components=%zu",
                  inventory.component_count) > 0);
  verify_format(&inventory, UMI_COMPONENT_REPORT_TEXT, expected);
  verify_format(&inventory, UMI_COMPONENT_REPORT_MARKDOWN,
                "# Umicom Framework Component Inventory");
  assert(snprintf(expected, sizeof(expected), "\"component_count\":%zu",
                  inventory.component_count) > 0);
  verify_format(&inventory, UMI_COMPONENT_REPORT_JSON, expected);

  umi_component_query_init(&query);
  query.domain_id = "trading";
  assert(umi_component_inventory_report_write(&inventory, &query, UMI_COMPONENT_REPORT_JSON, output,
                                              sizeof(output), &required) == UMI_STATUS_OK);
  assert(snprintf(expected, sizeof(expected), "\"selected_count\":%zu",
                  umi_application_component_domain_count("trading")) > 0);
  assert(strstr(output, expected) != NULL);
  assert(strstr(output, "umicom.treasury") == NULL);
  return 0;
}
