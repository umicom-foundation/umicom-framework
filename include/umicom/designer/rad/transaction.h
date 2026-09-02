/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/transaction.h
 *
 * PURPOSE:
 *   Track atomic designer transactions and mutation counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_TRANSACTION_H
#define UMICOM_DESIGNER_RAD_TRANSACTION_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad designer transaction data shared with callers of this public contract.
 */
typedef struct UmiRadDesignerTransaction {
    char transaction_id[UMI_RAD_ID_CAPACITY];
    UmiRadTransactionState state;
    size_t mutation_count;
    uint64_t revision;
} UmiRadDesignerTransaction;
/**
 * Initialise rad transaction from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_transaction_init(UmiRadDesignerTransaction *item);
/**
 * Check that rad transaction satisfies its contract before another service relies on it.
 */
int umi_rad_transaction_is_valid(const UmiRadDesignerTransaction *item);
#ifdef __cplusplus
}
#endif
#endif
