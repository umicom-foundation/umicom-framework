/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/diagnostic.h
 *
 * PURPOSE:
 *   Explain workspace recipe problems in plain language so designers and
 *   junior developers can fix a layout before an application tries to use it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_DIAGNOSTIC_H
#define UMICOM_APPLICATION_COMPONENT_DIAGNOSTIC_H

#include "umicom/application/component/draft.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_COMPONENT_DIAGNOSTIC_CAPACITY 64U
#define UMI_APPLICATION_COMPONENT_DIAGNOSTIC_MESSAGE_CAPACITY 320U

typedef enum UmiApplicationComponentDiagnosticSeverity {
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_INFORMATION = 1,
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_WARNING = 2,
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_ERROR = 3
} UmiApplicationComponentDiagnosticSeverity;

typedef enum UmiApplicationComponentDiagnosticCode {
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_MISSING_COMPONENT = 1,
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_UNSUPPORTED_FRONTEND = 2,
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_DUPLICATE_INSTANCE = 3,
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_DUPLICATE_SINGLETON = 4,
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_NO_VISIBLE_COMPONENT = 5,
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_NO_PRIMARY_COMPONENT = 6,
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_INVALID_WEIGHT = 7,
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_FLOATING_RESTRICTED = 8,
  UMI_APPLICATION_COMPONENT_DIAGNOSTIC_TOO_MANY_SLOTS = 9
} UmiApplicationComponentDiagnosticCode;

typedef struct UmiApplicationComponentDiagnostic {
  UmiApplicationComponentDiagnosticSeverity severity;
  UmiApplicationComponentDiagnosticCode code;
  size_t slot_index;
  char component_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char message[UMI_APPLICATION_COMPONENT_DIAGNOSTIC_MESSAGE_CAPACITY];
} UmiApplicationComponentDiagnostic;

typedef struct UmiApplicationComponentDiagnosticReport {
  UmiApplicationComponentDiagnostic entries[UMI_APPLICATION_COMPONENT_DIAGNOSTIC_CAPACITY];
  size_t count;
  size_t information_count;
  size_t warning_count;
  size_t error_count;
} UmiApplicationComponentDiagnosticReport;

void umi_application_component_diagnostic_report_init(
    UmiApplicationComponentDiagnosticReport *report);
const UmiApplicationComponentDiagnostic *umi_application_component_diagnostic_report_at(
    const UmiApplicationComponentDiagnosticReport *report, size_t index);
int umi_application_component_diagnostic_report_passed(
    const UmiApplicationComponentDiagnosticReport *report);

#ifdef __cplusplus
}
#endif

#endif
