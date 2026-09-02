/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/legal_entity.c
 *
 * PURPOSE:
 *   Implement legal entities shared by all financial applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/legal_entity.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_legal_entity_init(UmiLegalEntity *item,const char *id,const char *name,const char *code){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->entity_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_legal_entity_is_valid(const UmiLegalEntity *item){return item!=NULL&&umi_financial_id_is_valid(&item->entity_id)&&item->name[0]!='\0'&&item->code[0]!='\0';}
