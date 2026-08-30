/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/coverage.h
 *
 * PURPOSE:
 *   Calculate implementation and acceptance coverage across declared product
 *   features, panels and layouts without changing their canonical definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_COVERAGE_H
#define UMICOM_APPLICATION_PRODUCTISATION_COVERAGE_H

#include "umicom/application/productisation/evidence.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProductApplicationCoverage {
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    size_t asset_count;
    size_t foundation_count;
    size_t implemented_count;
    size_t adopted_count;
    size_t tested_count;
    size_t accepted_count;
    size_t blocked_count;
    size_t unevidenced_count;
    unsigned coverage_percent;
} UmiProductApplicationCoverage;

typedef struct UmiProductPortfolioCoverage {
    UmiProductApplicationCoverage
        applications[UMI_PRODUCTISATION_MAX_APPLICATIONS];
    size_t application_count;
    size_t asset_count;
    size_t accepted_count;
    size_t blocked_count;
    unsigned coverage_percent;
} UmiProductPortfolioCoverage;

UmiStatus umi_product_portfolio_coverage_build(
    const UmiProductisationEvidenceLedger *ledger,
    UmiProductPortfolioCoverage *out_coverage);
const UmiProductApplicationCoverage *umi_product_portfolio_coverage_find(
    const UmiProductPortfolioCoverage *coverage,
    const char *application_id);

#ifdef __cplusplus
}
#endif

#endif
