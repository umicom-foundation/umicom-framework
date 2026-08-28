/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/market_data_snapshot.c
 *
 * PURPOSE:
 *   Implement bounded scalar market-data snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/market_data_snapshot.h"

#include <math.h>
#include <string.h>
/* Initialize snapshot. */ UmiStatus umi_market_data_snapshot_init(UmiMarketDataSnapshot *s,const char *id){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);return umi_financial_id_assign(&s->snapshot_id,id);}
/* Add observation. */ UmiStatus umi_market_data_snapshot_add(UmiMarketDataSnapshot *s,const UmiMarketDataObservation *o){if(s==NULL||o==NULL||!umi_market_data_key_is_valid(&o->key)||!isfinite(o->value)||o->timestamp<0)return UMI_STATUS_INVALID_ARGUMENT;if(s->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;s->items[s->count++]=*o;return UMI_STATUS_OK;}
/* Validate snapshot. */ bool umi_market_data_snapshot_is_valid(const UmiMarketDataSnapshot *s){return s!=NULL&&umi_financial_id_is_valid(&s->snapshot_id)&&s->count<=UMI_FINANCIAL_CORE_MAX_ITEMS;}
