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

void umi_application_component_diagnostic_report_init(
    UmiApplicationComponentDiagnosticReport *report) {
  if (report != NULL)
    (void)memset(report, 0, sizeof(*report));
}

const UmiApplicationComponentDiagnostic *umi_application_component_diagnostic_report_at(
    const UmiApplicationComponentDiagnosticReport *report, size_t index) {
  return report != NULL && report->count <= UMI_APPLICATION_COMPONENT_DIAGNOSTIC_CAPACITY &&
                 index < report->count
             ? &report->entries[index]
             : NULL;
}

int umi_application_component_diagnostic_report_passed(
    const UmiApplicationComponentDiagnosticReport *report) {
  return report != NULL && report->count <= UMI_APPLICATION_COMPONENT_DIAGNOSTIC_CAPACITY &&
         report->error_count == 0U;
}
