/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/trade_event.c
 *
 * PURPOSE:
 *   Describe immutable trade lifecycle evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/trade_event.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_trade_event_init(UmiTradeEvent *item,const char *id,const char *name,const char *parent_id,UmiFinancialDate effective_date,uint32_t state){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->event_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_id_assign(&item->parent_id,parent_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_financial_date_is_valid(effective_date))return UMI_STATUS_INVALID_ARGUMENT; item->effective_date=effective_date; item->state=state; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_trade_event_is_valid(const UmiTradeEvent *item){return item!=NULL&&umi_financial_id_is_valid(&item->event_id)&&item->name[0]!='\0'&&umi_financial_id_is_valid(&item->parent_id)&&umi_financial_date_is_valid(item->effective_date);}
