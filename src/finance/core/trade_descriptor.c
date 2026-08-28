/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/trade_descriptor.c
 *
 * PURPOSE:
 *   Describe common trade booking identity and lifecycle state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/trade_descriptor.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_trade_descriptor_init(UmiTradeDescriptor *item,const char *id,const char *name,const char *parent_id,UmiFinancialDate effective_date,uint32_t state){UmiStatus st; if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->trade_id,id); if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); if(st!=UMI_STATUS_OK)return st; st=umi_financial_id_assign(&item->parent_id,parent_id); if(st!=UMI_STATUS_OK)return st; if(!umi_financial_date_is_valid(effective_date))return UMI_STATUS_INVALID_ARGUMENT; item->effective_date=effective_date; item->state=state; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_trade_descriptor_is_valid(const UmiTradeDescriptor *item){return item!=NULL&&umi_financial_id_is_valid(&item->trade_id)&&item->name[0]!='\0'&&umi_financial_id_is_valid(&item->parent_id)&&umi_financial_date_is_valid(item->effective_date);}
