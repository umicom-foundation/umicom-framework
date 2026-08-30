/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/trade_id.c
 *
 * PURPOSE:
 *   Implement typed trade id assignment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/trade_id.h"

/* Assign identifier. */ UmiStatus umi_trade_id_set(UmiTradeId *id,const char *value){if(id==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_financial_id_assign(&id->id,value);}
/* Validate identifier. */ bool umi_trade_id_is_valid(const UmiTradeId *id){return id!=NULL&&umi_financial_id_is_valid(&id->id);}
