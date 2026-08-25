/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/type_descriptor.c
 *
 * PURPOSE:
 *   Describe portable native compiler types, sizes, alignments and derived pointer/array relationships.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/type_descriptor.h"
#include <string.h>
UmiStatus umi_nc_type_primitive(UmiNativeTypeDescriptor *t,uint32_t id,UmiNativeTypeKind kind,size_t size,size_t align){if(t==NULL||id==0U||kind==UMI_NC_TYPE_INVALID||align==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(t,0,sizeof(*t));t->id=id;t->kind=kind;t->size=size;t->alignment=align;return UMI_STATUS_OK;}
UmiStatus umi_nc_type_pointer(UmiNativeTypeDescriptor *t,uint32_t id,uint32_t pointee,size_t psize){if(t==NULL||id==0U||pointee==0U||(psize!=4U&&psize!=8U))return UMI_STATUS_INVALID_ARGUMENT;memset(t,0,sizeof(*t));t->id=id;t->kind=UMI_NC_TYPE_POINTER;t->size=psize;t->alignment=psize;t->element_type_id=pointee;return UMI_STATUS_OK;}
UmiStatus umi_nc_type_array(UmiNativeTypeDescriptor *t,uint32_t id,uint32_t elem,size_t esize,size_t ealign,size_t count){if(t==NULL||id==0U||elem==0U||ealign==0U||count==0U)return UMI_STATUS_INVALID_ARGUMENT;if(esize!=0U&&count>SIZE_MAX/esize)return UMI_STATUS_CAPACITY_EXCEEDED;memset(t,0,sizeof(*t));t->id=id;t->kind=UMI_NC_TYPE_ARRAY;t->size=esize*count;t->alignment=ealign;t->element_type_id=elem;t->element_count=count;return UMI_STATUS_OK;}
bool umi_nc_type_compatible(const UmiNativeTypeDescriptor *a,const UmiNativeTypeDescriptor *b){if(a==NULL||b==NULL||a->kind!=b->kind||a->is_const!=b->is_const||a->is_volatile!=b->is_volatile)return false;if(a->kind==UMI_NC_TYPE_POINTER)return a->element_type_id==b->element_type_id;if(a->kind==UMI_NC_TYPE_ARRAY)return a->element_type_id==b->element_type_id&&a->element_count==b->element_count;return a->size==b->size&&a->alignment==b->alignment;}
