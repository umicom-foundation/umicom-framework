/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/curve_reference.c
 *
 * PURPOSE:
 *   Implement provider-neutral curve references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/curve_reference.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_curve_reference_init(UmiCurveReference *item,const char *id,const char *name,const char *code){UmiStatus st; if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->reference_id,id); if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); if(st!=UMI_STATUS_OK)return st; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_curve_reference_is_valid(const UmiCurveReference *item){return item!=NULL&&umi_financial_id_is_valid(&item->reference_id)&&item->name[0]!='\0'&&item->code[0]!='\0';}
