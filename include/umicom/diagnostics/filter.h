/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/filter.h
 *
 * PURPOSE:
 *   Publish the public filter contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_FILTER_H
#define UMICOM_DIAGNOSTICS_FILTER_H

#include "umicom/diagnostics/diagnostic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDiagnosticFilter {
    uint32_t severity_mask;
    uint32_t kind_mask;
    char source[UMI_DIAGNOSTIC_SOURCE_CAPACITY];
    char owner[UMI_DIAGNOSTIC_OWNER_CAPACITY];
    char text[256];
    int include_resolved;
    int include_transient;
} UmiDiagnosticFilter;

void umi_diagnostic_filter_init(UmiDiagnosticFilter *filter);
void umi_diagnostic_filter_set_minimum_severity(UmiDiagnosticFilter *filter,
                                                UmiDiagnosticSeverity severity);
int umi_diagnostic_filter_matches(const UmiDiagnosticFilter *filter,
                                  const UmiDiagnosticSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
