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
typedef struct UmiUiReactiveChangeTransaction {
    char transaction_id[UMI_UI_REACTIVE_ID_CAPACITY];
    UmiUiReactiveTransactionState state;
    size_t change_count;
    uint64_t base_revision;
} UmiUiReactiveChangeTransaction;
UmiStatus umi_ui_reactive_change_transaction_begin(UmiUiReactiveChangeTransaction *item,const char *id,uint64_t base_revision);
UmiStatus umi_ui_reactive_change_transaction_commit(UmiUiReactiveChangeTransaction *item);
UmiStatus umi_ui_reactive_change_transaction_rollback(UmiUiReactiveChangeTransaction *item);
#ifdef __cplusplus
}
#endif
#endif
