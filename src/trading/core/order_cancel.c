/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/order_cancel.c
 *
 * PURPOSE:
 *   Describe a cancellable order request with version control and reason code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_cancel.h"
#include <string.h>
/* Initialise and validate describe a cancellable order request with version control and reason code.. */
UmiStatus umi_trading_order_cancel_init(UmiTradingOrderCancel *value,const UmiFinancialId * client_order_id, uint64_t expected_version, uint32_t reason_code) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    if(client_order_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->client_order_id=*client_order_id;
    value->expected_version=expected_version;
    value->reason_code=reason_code;
    return umi_trading_order_cancel_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_order_cancel_valid(const UmiTradingOrderCancel *value) { return value!=NULL && (value->client_order_id.value[0]!='\0' && value->expected_version>0U); }
