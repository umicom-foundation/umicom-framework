/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/transaction_scope.h
 *
 * PURPOSE:
 *   Model nested transaction intent, rollback-only state and commit eligibility above canonical backend transactions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_TRANSACTION_SCOPE_H
#define UMICOM_DATA_ENTERPRISE_TRANSACTION_SCOPE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataTransactionScope { char transaction_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; uint32_t depth; bool active; bool rollback_only; bool committed; } UmiDataTransactionScope;
/* Begin a transaction scope at depth one. */ UmiStatus umi_data_transaction_scope_begin(UmiDataTransactionScope *scope,const char *transaction_id);
/* Enter a nested logical scope without creating hidden backend transactions. */ UmiStatus umi_data_transaction_scope_enter(UmiDataTransactionScope *scope);
/* Leave one nested scope or commit the outer scope when eligible. */ UmiStatus umi_data_transaction_scope_leave(UmiDataTransactionScope *scope,bool success);
/* Mark the entire scope rollback-only after an error. */ void umi_data_transaction_scope_mark_rollback(UmiDataTransactionScope *scope);

#ifdef __cplusplus
}
#endif
#endif
