/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/qualification_view.h
 * Purpose: Expose immutable qualification presentation snapshots for native frontends.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DISTRIBUTION_QUALIFICATION_VIEW_H
#define UMICOM_DISTRIBUTION_QUALIFICATION_VIEW_H
#include "umicom/distribution/runtime/qualification.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_QUALIFICATION_VIEW_PAGE_LIMIT 256U
typedef struct UmiQualificationView UmiQualificationView;
typedef enum UmiQualificationViewKind {
    UMI_QUALIFICATION_VIEW_BUILD, UMI_QUALIFICATION_VIEW_TEST_PROCESS,
    UMI_QUALIFICATION_VIEW_TARGET, UMI_QUALIFICATION_VIEW_TEST
} UmiQualificationViewKind;
typedef enum UmiQualificationViewFilter {
    UMI_QUALIFICATION_VIEW_ALL, UMI_QUALIFICATION_VIEW_PROBLEMS,
    UMI_QUALIFICATION_VIEW_MISSING, UMI_QUALIFICATION_VIEW_TESTS,
    UMI_QUALIFICATION_VIEW_TARGETS
} UmiQualificationViewFilter;
typedef struct UmiQualificationViewRow {
    size_t index;
    UmiQualificationViewKind kind;
    const char *name; /* Borrowed from the immutable view, never truncated. */
    uint32_t reasons;
    bool recorded;
    UmiReleaseQualificationResult result;
} UmiQualificationViewRow;
typedef struct UmiQualificationViewSummary {
    UmiReleaseQualificationIdentity identity;
    UmiReleaseQualificationSummary results;
    UmiReleaseImpactSummary selection;
    char source[65];
    size_t rows;
    bool savedReport; /* Historical report, never a resumed worker or current-source validation. */
} UmiQualificationViewSummary;
/** Capture on the run's owning thread while mutations are serialized. The
 * result owns every name, result and summary and can then be handed to another
 * thread. Later run changes/destruction do not affect it. Immutable reads can
 * be concurrent; destruction requires all readers to have stopped.
 * outView must address NULL; refusal preserves it. No I/O or job execution. */
UmiStatus UmiQualificationViewCapture(const UmiReleaseQualification *run, UmiQualificationView **outView);
void UmiQualificationViewDestroy(UmiQualificationView *view);
UmiStatus UmiQualificationViewDescribe(const UmiQualificationView *view, UmiQualificationViewSummary *outSummary);
/** Stable unfiltered index; NULL/out of range returns NULL. Targets represent
 * selection and share aggregate build evidence, not per-target execution proof.
 * Missing optional test-process evidence is visible but is not itself a failure. */
const UmiQualificationViewRow *UmiQualificationViewAt(const UmiQualificationView *view, size_t index);
/** Page at most PAGE_LIMIT copied rows. Their names remain borrowed from view.
 * offset is a filtered offset and may equal total. Zero capacity with NULL rows
 * measures total. Invalid inputs/offsets preserve every output. Problems includes
 * every recorded non-passing outcome, including skipped/disabled/not-run.
 * Missing includes required build/test rows, not optional runner evidence.
 * Summary blockers always remain relevant even when a filter has no rows. */
UmiStatus UmiQualificationViewPage(const UmiQualificationView *view, UmiQualificationViewFilter filter,
    size_t offset, UmiQualificationViewRow *rows, size_t capacity, size_t *outCount, size_t *outTotal);
const char *UmiQualificationViewStateText(const UmiQualificationView *view);
const char *UmiQualificationViewRowStateText(const UmiQualificationViewRow *row);
#ifdef __cplusplus
}
#endif
#endif
