/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostic_ui/navigation.h
 *
 * PURPOSE:
 *   Publish the public navigation contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTIC_UI_NAVIGATION_H
#define UMICOM_DIAGNOSTIC_UI_NAVIGATION_H

#include "umicom/diagnostics/query.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDiagnosticNavigation {
    uint64_t current_sequence;
    int wrap;
} UmiDiagnosticNavigation;

void umi_diagnostic_navigation_init(UmiDiagnosticNavigation *navigation);
UmiStatus umi_diagnostic_navigation_next(UmiDiagnosticNavigation *navigation,
                                         const UmiDiagnosticModel *model,
                                         const UmiDiagnosticFilter *filter,
                                         UmiDiagnosticSnapshot *out_diagnostic);
UmiStatus umi_diagnostic_navigation_previous(UmiDiagnosticNavigation *navigation,
                                             const UmiDiagnosticModel *model,
                                             const UmiDiagnosticFilter *filter,
                                             UmiDiagnosticSnapshot *out_diagnostic);

#ifdef __cplusplus
}
#endif
#endif
