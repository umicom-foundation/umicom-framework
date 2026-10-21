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
#include "umicom/document/coordinator.h"

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

/** Select the next/previous unresolved source diagnostic across the entire
 * retained model, not just its first query page. A nonzero backwards flag
 * reverses direction. Honours navigation->wrap and the caller's filter.
 * Source-less messages and non-file URI schemes are skipped. Call on the
 * model owner's thread; selection and outDiagnostic change only on success. */
UmiStatus UmiDiagnosticNavigationSource(UmiDiagnosticNavigation *navigation,
    const UmiDiagnosticModel *model, const UmiDiagnosticFilter *filter,
    int backwards, UmiDiagnosticSnapshot *outDiagnostic);

/** Open a diagnostic's local source through the existing document coordinator,
 * retaining unsaved drafts in all tabs. Relative paths need an absolute base
 * directory. No external program, URI handler or build is executed.
 * Lines/columns follow GoToPosition's one-based UTF-8 byte convention.
 * If the source changed and the line no longer exists, its tab stays open but
 * NOT_FOUND is returned; no text is edited, saved or discarded.
 * File opening and positioning record one successful history jump. A stale
 * line restores the previous active document when it still exists. */
UmiStatus UmiDiagnosticOpenSource(UmiDocumentCoordinator *documents,
    const UmiDiagnosticSnapshot *diagnostic, const char *baseDirectory,
    size_t *outOffset);

/** Select and open the next available source using the shared document owner.
 * Missing files or lines are skipped within one bounded traversal; filtering,
 * direction and wrap follow UmiDiagnosticNavigationSource. Other errors stop
 * the operation. A model revision change returns INVALID_STATE for refresh.
 * The navigation cursor and optional outputs change only after a successful
 * open. A stale line may leave its source tab open, as OpenSource documents;
 * no source text is edited, saved or discarded. Keep all owners alive through
 * this synchronous, owner-thread call, which may read local source files. */
UmiStatus UmiDiagnosticNavigateOpenSource(UmiDiagnosticNavigation *navigation,
    const UmiDiagnosticModel *model, const UmiDiagnosticFilter *filter, int backwards,
    UmiDocumentCoordinator *documents, const char *baseDirectory,
    UmiDiagnosticSnapshot *outDiagnostic, size_t *outOffset);

#ifdef __cplusplus
}
#endif
#endif
