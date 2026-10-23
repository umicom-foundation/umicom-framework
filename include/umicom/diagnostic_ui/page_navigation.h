/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostic_ui/page_navigation.h
 * PURPOSE: Clamp diagnostic list navigation without changing arbitrary-offset query semantics.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DIAGNOSTIC_UI_PAGE_NAVIGATION_H
#define UMICOM_DIAGNOSTIC_UI_PAGE_NAVIGATION_H
#include "umicom/diagnostic_ui/page.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDiagnosticPageNavigation {
    UmiDiagnosticQuery query;
    UmiDiagnosticPageSummary summary;
    size_t page_index, page_count, previous_offset, next_offset;
    int has_previous, has_next;
} UmiDiagnosticPageNavigation;
/* Capture a page aligned to query.limit (zero means 300), clamping an obsolete
 * offset to the last available page. At most two captures are attempted; a
 * model revision change cancels the operation. Neither caller output changes
 * on failure. The caller owns *out_page after success and must serialize model
 * access. The returned query is the normalized identity for page validation.
 * Existing UmiDiagnosticPageCreate retains its arbitrary-offset behavior. */
UmiStatus UmiDiagnosticPageCreateWindow(const UmiDiagnosticModel *model,
    const UmiDiagnosticQuery *query, UmiDiagnosticPage **out_page,
    UmiDiagnosticPageNavigation *out_navigation);
#ifdef __cplusplus
}
#endif
#endif
