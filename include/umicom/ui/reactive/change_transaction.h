/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/change_transaction.h
 *
 * PURPOSE:
 *   Represent a bounded reactive state transaction with explicit commit/rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CHANGE_TRANSACTION_H
#define UMICOM_UI_REACTIVE_CHANGE_TRANSACTION_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive change transaction data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveChangeTransaction {
    char transaction_id[UMI_UI_REACTIVE_ID_CAPACITY];
    UmiUiReactiveTransactionState state;
    size_t change_count;
    uint64_t base_revision;
} UmiUiReactiveChangeTransaction;
/**
 * Provide the ui reactive change transaction begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_reactive_change_transaction_begin(UmiUiReactiveChangeTransaction *item,const char *id,uint64_t base_revision);
/**
 * Provide the ui reactive change transaction commit operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_reactive_change_transaction_commit(UmiUiReactiveChangeTransaction *item);
/**
 * Provide the ui reactive change transaction rollback operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_reactive_change_transaction_rollback(UmiUiReactiveChangeTransaction *item);
#ifdef __cplusplus
}
#endif
#endif
