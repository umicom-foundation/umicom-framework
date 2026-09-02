/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/pricing_context.c
 *
 * PURPOSE:
 *   Implement pricing context identity and valuation date.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/pricing_context.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_pricing_context_init(UmiPricingContext *item,const char *id,const char *name,const char *code,UmiFinancialDate effective_date){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->context_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_financial_date_is_valid(effective_date))return UMI_STATUS_INVALID_ARGUMENT; item->effective_date=effective_date; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_pricing_context_is_valid(const UmiPricingContext *item){return item!=NULL&&umi_financial_id_is_valid(&item->context_id)&&item->name[0]!='\0'&&item->code[0]!='\0'&&umi_financial_date_is_valid(item->effective_date);}
