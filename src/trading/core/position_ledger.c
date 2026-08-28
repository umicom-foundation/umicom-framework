/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/position_ledger.c
 *
 * PURPOSE:
 *   Maintain bounded per-instrument lot positions for brokerage and exchange services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/position_ledger.h"

#include <string.h>
void umi_trading_position_ledger_init(UmiTradingPositionLedger *ledger){if(ledger!=NULL)memset(ledger,0,sizeof *ledger);}
UmiTradingQuantityLots umi_trading_position_ledger_get(const UmiTradingPositionLedger *ledger,const UmiFinancialId *instrument_id){if(ledger==NULL||instrument_id==NULL)return 0;for(size_t i=0;i<ledger->count;i++)if(umi_trading_core_id_equal(&ledger->items[i].instrument_id,instrument_id))return ledger->items[i].lots;return 0;}
UmiStatus umi_trading_position_ledger_apply(UmiTradingPositionLedger *ledger,const UmiFinancialId *instrument_id,UmiSide side,UmiTradingQuantityLots lots){if(ledger==NULL||instrument_id==NULL||instrument_id->value[0]=='\0'||lots<=0||(side!=UMI_SIDE_BUY&&side!=UMI_SIDE_SELL))return UMI_STATUS_INVALID_ARGUMENT;size_t idx=ledger->count;for(size_t i=0;i<ledger->count;i++)if(umi_trading_core_id_equal(&ledger->items[i].instrument_id,instrument_id)){idx=i;break;}if(idx==ledger->count){if(ledger->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;ledger->items[idx].instrument_id=*instrument_id;ledger->items[idx].lots=0;ledger->count++;}int64_t delta=side==UMI_SIDE_BUY?lots:-lots;return umi_trading_core_add_i64(ledger->items[idx].lots,delta,&ledger->items[idx].lots);}
