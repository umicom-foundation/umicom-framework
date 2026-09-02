/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_queue.h
 *
 * PURPOSE:
 *   Maintain bounded settlement obligations and prevent duplicate settlement identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_QUEUE_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_QUEUE_H
#include "umicom/finance/treasury/settlement_obligation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury settlement queue data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasurySettlementQueue {
    UmiTreasurySettlementObligation items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasurySettlementQueue;
/**
 * Initialise treasury settlement queue from caller-provided values so later operations
 * receive a known state.
 */
void umi_treasury_settlement_queue_init(UmiTreasurySettlementQueue *registry);
/**
 * Add treasury settlement queue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_treasury_settlement_queue_add(UmiTreasurySettlementQueue *registry, const UmiTreasurySettlementObligation *item);
/**
 * Find treasury settlement queue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiTreasurySettlementObligation *umi_treasury_settlement_queue_find(const UmiTreasurySettlementQueue *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
