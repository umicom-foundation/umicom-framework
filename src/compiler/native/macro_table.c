/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/macro_table.c
 *
 * PURPOSE:
 *   Maintain a bounded macro catalogue with replaceable definitions and deterministic revision tracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/macro_table.h"
#include <string.h>
/*
 * Initialise nc macro table from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_macro_table_init(UmiNativeMacroTable *t){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(t!=NULL) memset(t,0,sizeof(*t)); }
/*
 * Find nc macro table while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiNativeMacro *umi_nc_macro_table_find(const UmiNativeMacroTable *t,const char *name){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||name==NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<t->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(t->items[i].name,name)==0) return &t->items[i]; return NULL; }
/*
 * Provide the nc macro table define operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_macro_table_define(UmiNativeMacroTable *t,const UmiNativeMacro *m){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||m==NULL) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_macro_validate(m)!=UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<t->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(t->items[i].name,m->name)==0){t->items[i]=*m;t->revision++;return UMI_STATUS_OK;} /* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->count>=UMI_NC_MAX_MACROS) return UMI_STATUS_CAPACITY_EXCEEDED; t->items[t->count++]=*m;t->revision++;return UMI_STATUS_OK; }
/*
 * Provide the nc macro table undef operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_macro_table_undef(UmiNativeMacroTable *t,const char *name){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||name==NULL) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<t->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(t->items[i].name,name)==0){ /* Visit each bounded item once so every record receives the same rule. */ for(size_t j=i+1U;j<t->count;j++) t->items[j-1U]=t->items[j]; t->count--;t->revision++;return UMI_STATUS_OK;} return UMI_STATUS_NOT_FOUND; }
