/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/position_ledger.c
 *
 * PURPOSE:
 *   Maintain bounded per-instrument lot positions for brokerage and exchange services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/position_ledger.h"

#include <string.h>
/*
 * Initialise trading position ledger from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_position_ledger_init(UmiTradingPositionLedger *ledger){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ledger!=NULL)memset(ledger,0,sizeof *ledger);}
/*
 * Provide the trading position ledger get operation used by this module and its client
 * applications.
 */
UmiTradingQuantityLots umi_trading_position_ledger_get(const UmiTradingPositionLedger *ledger,const UmiFinancialId *instrument_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ledger==NULL||instrument_id==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<ledger->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_id_equal(&ledger->items[i].instrument_id,instrument_id))return ledger->items[i].lots;return 0;}
/*
 * Perform trading position ledger through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_trading_position_ledger_apply(UmiTradingPositionLedger *ledger,const UmiFinancialId *instrument_id,UmiSide side,UmiTradingQuantityLots lots){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ledger==NULL||instrument_id==NULL||instrument_id->value[0]=='\0'||lots<=0||(side!=UMI_SIDE_BUY&&side!=UMI_SIDE_SELL))return UMI_STATUS_INVALID_ARGUMENT;size_t idx=ledger->count;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<ledger->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_id_equal(&ledger->items[i].instrument_id,instrument_id)){idx=i;break;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(idx==ledger->count){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ledger->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;ledger->items[idx].instrument_id=*instrument_id;ledger->items[idx].lots=0;ledger->count++;}int64_t delta=side==UMI_SIDE_BUY?lots:-lots;return umi_trading_core_add_i64(ledger->items[idx].lots,delta,&ledger->items[idx].lots);}
