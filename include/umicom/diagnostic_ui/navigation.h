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

/**
 * Represent the diagnostic navigation data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticNavigation {
    uint64_t current_sequence;
    int wrap;
} UmiDiagnosticNavigation;

/**
 * Initialise diagnostic navigation from caller-provided values so later operations receive
 * a known state.
 */
void umi_diagnostic_navigation_init(UmiDiagnosticNavigation *navigation);
/**
 * Provide the diagnostic navigation next operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_navigation_next(UmiDiagnosticNavigation *navigation,
                                         const UmiDiagnosticModel *model,
                                         const UmiDiagnosticFilter *filter,
                                         UmiDiagnosticSnapshot *out_diagnostic);
/**
 * Provide the diagnostic navigation previous operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_navigation_previous(UmiDiagnosticNavigation *navigation,
                                             const UmiDiagnosticModel *model,
                                             const UmiDiagnosticFilter *filter,
                                             UmiDiagnosticSnapshot *out_diagnostic);

#ifdef __cplusplus
}
#endif
#endif
