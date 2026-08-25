/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/transaction_scope.c
 *
 * PURPOSE:
 *   Model nested transaction intent, rollback-only state and commit eligibility above canonical backend transactions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/transaction_scope.h"
#include <string.h>

/* Scope state coordinates callers while canonical data/transaction remains the backend authority. */ UmiStatus umi_data_transaction_scope_begin(UmiDataTransactionScope *scope,const char *transaction_id){UmiStatus s;if(scope==NULL||transaction_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(scope,0,sizeof(*scope));s=umi_data_enterprise_copy_text(scope->transaction_id,sizeof(scope->transaction_id),transaction_id);if(s!=UMI_STATUS_OK)return s;scope->depth=1U;scope->active=true;return UMI_STATUS_OK;}
/* Nested scopes share rollback semantics with the outer transaction. */ UmiStatus umi_data_transaction_scope_enter(UmiDataTransactionScope *scope){if(scope==NULL||!scope->active||scope->committed)return UMI_STATUS_INVALID_STATE;if(scope->depth==UINT32_MAX)return UMI_STATUS_CAPACITY_EXCEEDED;scope->depth++;return UMI_STATUS_OK;}
/* A failed nested scope marks rollback-only; only a successful outer leave may commit. */ UmiStatus umi_data_transaction_scope_leave(UmiDataTransactionScope *scope,bool success){if(scope==NULL||!scope->active||scope->depth==0U)return UMI_STATUS_INVALID_STATE;if(!success)scope->rollback_only=true;if(scope->depth>1U){scope->depth--;return success?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;}scope->depth=0U;scope->active=false;if(scope->rollback_only||!success)return UMI_STATUS_INVALID_STATE;scope->committed=true;return UMI_STATUS_OK;}
/* Rollback-only is monotonic for the lifetime of a scope. */ void umi_data_transaction_scope_mark_rollback(UmiDataTransactionScope *scope){if(scope!=NULL&&scope->active)scope->rollback_only=true;}
