/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/gap_analysis.h
 *
 * PURPOSE:
 *   Identify missing reusable capabilities/components, thin-application
 *   adoption work, external adapters and acceptance-evidence gaps.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_GAP_ANALYSIS_H
#define UMICOM_APPLICATION_PRODUCTISATION_GAP_ANALYSIS_H

#include "umicom/application/productisation/capability_matrix.h"
#include "umicom/application/productisation/evidence.h"

#ifdef __cplusplus
extern "C" {
#endif

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

UmiStatus umi_productisation_gap_analysis_run(
    const UmiProductCapabilityMatrix *matrix,
    const UmiProductisationEvidenceLedger *ledger,
    UmiProductisationGapReport *out_report);
const UmiProductisationGap *umi_productisation_gap_report_at(
    const UmiProductisationGapReport *report,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
