/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/diagnostic.c
 *
 * PURPOSE:
 *   Maintain bounded diagnostic totals and expose safe report queries for
 *   workspace designers, command-line tools and application settings screens.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/diagnostic.h"

#include <string.h>

/*
 * Initialise application component diagnostic report from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_component_diagnostic_report_init(
    UmiApplicationComponentDiagnosticReport *report) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (report != NULL)
    (void)memset(report, 0, sizeof(*report));
}

/*
 * Find application component diagnostic report while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationComponentDiagnostic *umi_application_component_diagnostic_report_at(
    const UmiApplicationComponentDiagnosticReport *report, size_t index) {
  return report != NULL && report->count <= UMI_APPLICATION_COMPONENT_DIAGNOSTIC_CAPACITY &&
                 index < report->count
             ? &report->entries[index]
             : NULL;
}

/*
 * Provide the application component diagnostic report passed operation used by this module
 * and its client applications.
 */
int umi_application_component_diagnostic_report_passed(
    const UmiApplicationComponentDiagnosticReport *report) {
  return report != NULL && report->count <= UMI_APPLICATION_COMPONENT_DIAGNOSTIC_CAPACITY &&
         report->error_count == 0U;
}
