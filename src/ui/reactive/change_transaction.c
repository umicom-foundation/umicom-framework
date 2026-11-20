/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/change_transaction.c
 *
 * PURPOSE:
 *   Implement a bounded reactive state transaction with explicit commit/rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/change_transaction.h"
#include "umicom/base/text.h"
#include <string.h>
/* Begin one explicit state transaction from a known base revision. */
/* Preserve the earlier initializer for review. The active path stages the
 * bounded ID before publishing so a caller may reuse its own transaction ID. */
#if 0
UmiStatus umi_ui_reactive_change_transaction_begin(UmiUiReactiveChangeTransaction *t,const char *id,uint64_t base){size_t n;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!t||!id||id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;n=strlen(id);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>=sizeof t->transaction_id)return UMI_STATUS_INVALID_ARGUMENT;memset(t,0,sizeof *t);memcpy(t->transaction_id,id,n+1U);t->state=UMI_UI_REACTIVE_TRANSACTION_ACTIVE;t->base_revision=base;return UMI_STATUS_OK;}
#endif
UmiStatus umi_ui_reactive_change_transaction_begin(UmiUiReactiveChangeTransaction *item,
    const char *id, uint64_t base_revision)
{
    if (item == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    UmiUiReactiveChangeTransaction candidate = {0};
    if (umi_text_replace(candidate.transaction_id, sizeof candidate.transaction_id, id) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    candidate.state = UMI_UI_REACTIVE_TRANSACTION_ACTIVE;
    candidate.base_revision = base_revision;
    *item = candidate;
    return UMI_STATUS_OK;
}
/* Commit an active transaction. */
UmiStatus umi_ui_reactive_change_transaction_commit(UmiUiReactiveChangeTransaction *t){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!t||t->state!=UMI_UI_REACTIVE_TRANSACTION_ACTIVE)return UMI_STATUS_INVALID_STATE;t->state=UMI_UI_REACTIVE_TRANSACTION_COMMITTED;return UMI_STATUS_OK;}
/* Roll back an active transaction. */
UmiStatus umi_ui_reactive_change_transaction_rollback(UmiUiReactiveChangeTransaction *t){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!t||t->state!=UMI_UI_REACTIVE_TRANSACTION_ACTIVE)return UMI_STATUS_INVALID_STATE;t->state=UMI_UI_REACTIVE_TRANSACTION_ROLLED_BACK;t->change_count=0U;return UMI_STATUS_OK;}
