/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostic_ui/page.h
 * PURPOSE: Retain a bounded diagnostic page independently of native callbacks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTIC_UI_PAGE_H
#define UMICOM_DIAGNOSTIC_UI_PAGE_H
#include "umicom/diagnostics/query.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_DIAGNOSTIC_PAGE_LIMIT 300U
typedef struct UmiDiagnosticPage UmiDiagnosticPage;
typedef struct UmiDiagnosticPageSummary {
    uint64_t revision;
    size_t count;
    size_t total_matches;
    size_t active;
    size_t resolved;
    size_t warnings;
    size_t errors;
    size_t fatal;
    int has_more;
} UmiDiagnosticPageSummary;
/** Copy query results and active severity counts before publishing widgets.
 * Zero limit means PAGE_LIMIT; larger limits are rejected. Offset, filtering and
 * ordering follow the diagnostic query service. Storage is heap allocated.
 * Failure leaves out_page unchanged. A concurrent model change cancels capture;
 * callers schedule a fresh capture rather than rendering mixed revisions.
 * The model must remain alive throughout capture and every validation call. */
UmiStatus UmiDiagnosticPageCreate(const UmiDiagnosticModel *model,
    const UmiDiagnosticQuery *query, UmiDiagnosticPage **out_page);
void UmiDiagnosticPageDestroy(UmiDiagnosticPage *page);
UmiStatus UmiDiagnosticPageReadSummary(const UmiDiagnosticPage *page,
    UmiDiagnosticPageSummary *out_summary);
/** Return an immutable row borrowed until page destruction. No model access. */
const UmiDiagnosticSnapshot *UmiDiagnosticPageItem(const UmiDiagnosticPage *page, size_t index);
/** Check the same live model, revision and query before/after native callbacks.
 * This is an observation, not a lock or an action authorization. The host must
 * separately check its window lifetime and filter-change generation, including
 * edits away from and back to the same query. Retained copies remain readable
 * even after validation fails. Invalid input or mismatch never changes page. */
UmiStatus UmiDiagnosticPageValidate(const UmiDiagnosticPage *page,
    const UmiDiagnosticModel *model, const UmiDiagnosticQuery *query);
#ifdef __cplusplus
}
#endif
#endif
