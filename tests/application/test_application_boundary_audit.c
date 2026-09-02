/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_application_boundary_audit.c
 *
 * PURPOSE:
 *   Prove the built-in portfolio conforms and direct application dependencies
 *   are rejected with explicit architecture evidence.
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

#include "umicom/application/application.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiApplicationAuditReport report;
  const UmiApplicationDefinition *applications[2];
  UmiApplicationDependency forbidden = {"org.umicom.studio", UMI_APPLICATION_DEPENDENCY_APPLICATION,
                                        "org.umicom.trader"};

  assert(umi_application_portfolio_audit(&report) == UMI_STATUS_OK);
  /* Apply this branch only when its contract condition is satisfied. */
  if (!report.passed) {
    size_t finding_index;
    /* Emit the exact architecture rule so focused test linkage cannot
     * conceal a missing capability or invalid component projection. */
    for (finding_index = 0U; finding_index < report.finding_count; ++finding_index) {
      (void)fprintf(stderr, "%s|%s|%s\n", report.findings[finding_index].rule_id,
                    report.findings[finding_index].subject, report.findings[finding_index].message);
    }
  }
  assert(report.passed);
  assert(report.error_count == 0U);
  assert(report.application_count == umi_application_portfolio_count());
  assert(report.capability_reference_count > report.application_count);

  applications[0] = umi_application_portfolio_find("org.umicom.studio");
  applications[1] = umi_application_portfolio_find("org.umicom.trader");
  assert(umi_application_boundary_audit(applications, 2U, &forbidden, 1U, &report) ==
         UMI_STATUS_OK);
  assert(!report.passed);
  assert(report.error_count == 1U);
  assert(report.finding_count == 1U);
  return 0;
}
