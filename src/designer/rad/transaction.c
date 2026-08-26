/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/transaction.c
 *
 * PURPOSE:
 *   Track atomic designer transactions and mutation counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/transaction.h"
#include <string.h>
UmiStatus umi_rad_transaction_init(UmiRadDesignerTransaction *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->transaction_id, sizeof item->transaction_id, "transaction");
    return UMI_STATUS_OK;
}
int umi_rad_transaction_is_valid(const UmiRadDesignerTransaction *item){if(item==NULL)return 0;return umi_rad_id_valid(item->transaction_id);}
