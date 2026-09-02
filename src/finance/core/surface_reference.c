/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/surface_reference.c
 *
 * PURPOSE:
 *   Implement provider-neutral surface references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/surface_reference.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_surface_reference_init(UmiSurfaceReference *item,const char *id,const char *name,const char *code){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->reference_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_surface_reference_is_valid(const UmiSurfaceReference *item){return item!=NULL&&umi_financial_id_is_valid(&item->reference_id)&&item->name[0]!='\0'&&item->code[0]!='\0';}
