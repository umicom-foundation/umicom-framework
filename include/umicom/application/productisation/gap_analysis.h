/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/gap_analysis.h
 *
 * PURPOSE:
 *   Identify missing reusable capabilities/components, thin-application
 *   adoption work, external adapters and acceptance-evidence gaps.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_GAP_ANALYSIS_H
#define UMICOM_APPLICATION_PRODUCTISATION_GAP_ANALYSIS_H

#include "umicom/application/productisation/capability_matrix.h"
#include "umicom/application/productisation/evidence.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the productisation gap data shared with callers of this public contract.
 */
typedef struct UmiProductisationGap {
    UmiProductisationGapKind kind;
    UmiProductisationSeverity severity;
    UmiProductisationOwner owner;
    UmiProductisationAssetKind asset_kind;
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char asset_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char capability_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char summary[UMI_PRODUCTISATION_TEXT_CAPACITY];
    size_t affected_application_count;
} UmiProductisationGap;

/**
 * Represent the productisation gap report data shared with callers of this public
 * contract.
 */
typedef struct UmiProductisationGapReport {
    UmiProductisationGap gaps[UMI_PRODUCTISATION_MAX_GAPS];
    size_t gap_count;
    size_t framework_gap_count;
    size_t application_gap_count;
    size_t external_gap_count;
    size_t blocker_count;
    size_t high_count;
    size_t medium_count;
    size_t low_count;
    int capacity_exhausted;
} UmiProductisationGapReport;

/**
 * Compare capability use with recorded evidence and identify unfinished work.
 *
 * The report owns copies of all resulting gap descriptions and does not retain
 * pointers to the matrix or ledger.
 *
 * @param matrix Canonical capability use across the application portfolio.
 * @param ledger Current implementation and acceptance evidence.
 * @param out_report Receives gaps and severity totals.
 * @return `UMI_STATUS_OK` on success, or a validation or capacity status.
 */
UmiStatus umi_productisation_gap_analysis_run(
    const UmiProductCapabilityMatrix *matrix,
    const UmiProductisationEvidenceLedger *ledger,
    UmiProductisationGapReport *out_report);

/**
 * Read one gap from a completed analysis report.
 *
 * @param report Report that owns the returned gap.
 * @param index Zero-based position smaller than `report->gap_count`.
 * @return Pointer into `report`, or `NULL` for invalid input or position.
 */
const UmiProductisationGap *umi_productisation_gap_report_at(
    const UmiProductisationGapReport *report,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
