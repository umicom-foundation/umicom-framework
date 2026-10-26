/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/comparison_private.h
 *
 * PURPOSE:
 *   Own source rows independently from ordered comparison entries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BANK_FEED_COMPARISON_PRIVATE_H
#define UMICOM_BANK_FEED_COMPARISON_PRIVATE_H
#include "umicom/bank_feed/comparison.h"
typedef struct BankComparisonEntry {
    size_t before, after;
    UmiBankFeedComparisonKind kind;
    uint32_t changes;
} BankComparisonEntry;
struct UmiBankFeedComparison {
    UmiBankFeedComparisonSummary summary;
    UmiBankFeedRow *before, *after;
    BankComparisonEntry *entries;
};
#endif
