/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/macro_table.c
 *
 * PURPOSE:
 *   Maintain a bounded macro catalogue with replaceable definitions and deterministic revision tracking.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/macro_table.h"
#include <string.h>
void umi_nc_macro_table_init(UmiNativeMacroTable *t){ if(t!=NULL) memset(t,0,sizeof(*t)); }
const UmiNativeMacro *umi_nc_macro_table_find(const UmiNativeMacroTable *t,const char *name){ if(t==NULL||name==NULL) return NULL; for(size_t i=0U;i<t->count;i++) if(strcmp(t->items[i].name,name)==0) return &t->items[i]; return NULL; }
UmiStatus umi_nc_macro_table_define(UmiNativeMacroTable *t,const UmiNativeMacro *m){ if(t==NULL||m==NULL) return UMI_STATUS_INVALID_ARGUMENT; if(umi_nc_macro_validate(m)!=UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT; for(size_t i=0U;i<t->count;i++) if(strcmp(t->items[i].name,m->name)==0){t->items[i]=*m;t->revision++;return UMI_STATUS_OK;} if(t->count>=UMI_NC_MAX_MACROS) return UMI_STATUS_CAPACITY_EXCEEDED; t->items[t->count++]=*m;t->revision++;return UMI_STATUS_OK; }
UmiStatus umi_nc_macro_table_undef(UmiNativeMacroTable *t,const char *name){ if(t==NULL||name==NULL) return UMI_STATUS_INVALID_ARGUMENT; for(size_t i=0U;i<t->count;i++) if(strcmp(t->items[i].name,name)==0){ for(size_t j=i+1U;j<t->count;j++) t->items[j-1U]=t->items[j]; t->count--;t->revision++;return UMI_STATUS_OK;} return UMI_STATUS_NOT_FOUND; }
