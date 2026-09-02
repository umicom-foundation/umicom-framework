/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/type_table.c
 *
 * PURPOSE:
 *   Maintain canonical type descriptors with stable IDs and structural lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/type_table.h"
#include <string.h>
/*
 * Initialise nc type table from caller-provided values so later operations receive a known
 * state.
 */
void umi_nc_type_table_init(UmiNativeTypeTable *t){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t!=NULL)memset(t,0,sizeof(*t));}
/* Find nc type table while leaving the underlying catalogue or model owned by this module. */
const UmiNativeTypeDescriptor *umi_nc_type_table_find(const UmiNativeTypeTable *t,uint32_t id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||id==0U)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<t->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->items[i].id==id)return &t->items[i];return NULL;}
/*
 * Provide the nc type table find compatible operation used by this module and its client
 * applications.
 */
const UmiNativeTypeDescriptor *umi_nc_type_table_find_compatible(const UmiNativeTypeTable *t,const UmiNativeTypeDescriptor *type){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||type==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<t->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_type_compatible(&t->items[i],type))return &t->items[i];return NULL;}
/* Add nc type table only after its inputs and available capacity have been checked. */
UmiStatus umi_nc_type_table_add(UmiNativeTypeTable *t,const UmiNativeTypeDescriptor *type){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||type==NULL||type->id==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_type_table_find(t,type->id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->count>=UMI_NC_MAX_TYPES)return UMI_STATUS_CAPACITY_EXCEEDED;t->items[t->count++]=*type;t->revision++;return UMI_STATUS_OK;}
