/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/qualification_compare.h
 * Purpose: Compare closed qualification evidence without attributing changed-input outcomes to a cause.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DISTRIBUTION_QUALIFICATION_COMPARE_H
#define UMICOM_DISTRIBUTION_QUALIFICATION_COMPARE_H
#include "umicom/distribution/runtime/qualification_view.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiQualificationComparison UmiQualificationComparison;
typedef enum UmiQualificationChange {
    UMI_QUALIFICATION_UNCHANGED, UMI_QUALIFICATION_ADDED, UMI_QUALIFICATION_REMOVED,
    UMI_QUALIFICATION_REGRESSED, UMI_QUALIFICATION_RECOVERED, UMI_QUALIFICATION_RESULT_CHANGED
} UmiQualificationChange;
typedef struct UmiQualificationDifference {
    size_t index;
    UmiQualificationViewKind kind;
    const char *name; /* Owned by comparison; exact identity, never a UI label. */
    UmiQualificationChange change;
    bool beforePresent, afterPresent, beforeRecorded, afterRecorded;
    UmiReleaseQualificationResult before, after;
} UmiQualificationDifference;
typedef struct UmiQualificationComparisonSummary {
    UmiQualificationViewSummary before, after;
    size_t rows, unchanged, added, removed, regressed, recovered, resultChanged;
    bool sameRecordedPlan, sameExecution;
} UmiQualificationComparisonSummary;
/** Compare two closed immutable views. Open attempts return UNAVAILABLE.
 * Copy identities, names and results; neither input is borrowed after return.
 * Order: aggregate build, optional test process, then exact test names. Selected
 * targets remain in the two summaries and are not mistaken for execution rows.
 * Added/removed means test coverage changed. Regressed means a previously
 * recorded pass now has missing or non-passing evidence; recovered is the reverse.
 * Other status/exit/outcome changes remain RESULT_CHANGED. Duration and log digest
 * differences are preserved in rows but do not by themselves change this category.
 * Names can match across different inputs: inspect sameRecordedPlan and both
 * summary blockers before drawing conclusions. Comparison is descriptive, never
 * causal attribution, fresh qualification, automatic acceptance or retry.
 * Saved-report provenance remains on each copied summary. No I/O occurs.
 * *outComparison must be NULL; output is preserved on error. */
UmiStatus UmiQualificationCompare(const UmiQualificationView *before, const UmiQualificationView *after,
    UmiQualificationComparison **outComparison);
void UmiQualificationComparisonDestroy(UmiQualificationComparison *comparison);
UmiStatus UmiQualificationComparisonDescribe(const UmiQualificationComparison *comparison,
    UmiQualificationComparisonSummary *outSummary);
size_t UmiQualificationComparisonCount(const UmiQualificationComparison *comparison);
const UmiQualificationDifference *UmiQualificationComparisonAt(const UmiQualificationComparison *comparison, size_t index);
const char *UmiQualificationChangeText(UmiQualificationChange change);
#ifdef __cplusplus
}
#endif
#endif
