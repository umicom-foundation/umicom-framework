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
void umi_nc_type_table_init(UmiNativeTypeTable *t){if(t!=NULL)memset(t,0,sizeof(*t));}
const UmiNativeTypeDescriptor *umi_nc_type_table_find(const UmiNativeTypeTable *t,uint32_t id){if(t==NULL||id==0U)return NULL;for(size_t i=0U;i<t->count;i++)if(t->items[i].id==id)return &t->items[i];return NULL;}
const UmiNativeTypeDescriptor *umi_nc_type_table_find_compatible(const UmiNativeTypeTable *t,const UmiNativeTypeDescriptor *type){if(t==NULL||type==NULL)return NULL;for(size_t i=0U;i<t->count;i++)if(umi_nc_type_compatible(&t->items[i],type))return &t->items[i];return NULL;}
UmiStatus umi_nc_type_table_add(UmiNativeTypeTable *t,const UmiNativeTypeDescriptor *type){if(t==NULL||type==NULL||type->id==0U)return UMI_STATUS_INVALID_ARGUMENT;if(umi_nc_type_table_find(t,type->id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(t->count>=UMI_NC_MAX_TYPES)return UMI_STATUS_CAPACITY_EXCEEDED;t->items[t->count++]=*type;t->revision++;return UMI_STATUS_OK;}
