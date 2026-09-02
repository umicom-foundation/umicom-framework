/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/transaction.c
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
#include "umicom/designer/visual_designer/transaction.h"
#include <string.h>
/*
 * Initialise visual designer transaction from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_transaction_init(UmiRadDesignerTransaction *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->transaction_id, sizeof item->transaction_id, "transaction");
    return UMI_STATUS_OK;
}
/* Check that visual designer transaction satisfies its contract before another service relies on it. */
int umi_rad_transaction_is_valid(const UmiRadDesignerTransaction *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->transaction_id);}
