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

/**
 * Represent the diagnostic filter data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticFilter {
    uint32_t severity_mask;
    uint32_t kind_mask;
    char source[UMI_DIAGNOSTIC_SOURCE_CAPACITY];
    char owner[UMI_DIAGNOSTIC_OWNER_CAPACITY];
    char text[256];
    int include_resolved;
    int include_transient;
} UmiDiagnosticFilter;

/**
 * Initialise diagnostic filter from caller-provided values so later operations receive a
 * known state.
 */
void umi_diagnostic_filter_init(UmiDiagnosticFilter *filter);
/**
 * Provide the diagnostic filter set minimum severity operation used by this module and its
 * client applications.
 */
void umi_diagnostic_filter_set_minimum_severity(UmiDiagnosticFilter *filter,
                                                UmiDiagnosticSeverity severity);
/**
 * Provide the diagnostic filter matches operation used by this module and its client
 * applications.
 */
int umi_diagnostic_filter_matches(const UmiDiagnosticFilter *filter,
                                  const UmiDiagnosticSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
