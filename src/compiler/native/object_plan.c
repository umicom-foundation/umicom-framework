/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/object_plan.c
 *
 * PURPOSE:
 *   Aggregate object sections, relocations and object-format intent before backend serialization.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/object_plan.h"
#include <string.h>
void umi_nc_object_plan_init(UmiNativeObjectPlan *p,UmiNativeObjectFormat f){if(p!=NULL){memset(p,0,sizeof(*p));p->format=f;}}
UmiStatus umi_nc_object_plan_add_section(UmiNativeObjectPlan *p,const UmiNativeObjectSection *s){if(p==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(p->section_count>=UMI_NC_MAX_OBJECT_SECTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;for(size_t i=0U;i<p->section_count;i++)if(strcmp(p->sections[i].name,s->name)==0)return UMI_STATUS_ALREADY_EXISTS;p->sections[p->section_count++]=*s;return UMI_STATUS_OK;}
UmiStatus umi_nc_object_plan_add_relocation(UmiNativeObjectPlan *p,const UmiNativeRelocation *r){if(p==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;UmiStatus st=umi_nc_relocation_validate(r,p->format);if(st!=UMI_STATUS_OK)return st;if(p->relocation_count>=UMI_NC_MAX_RELOCATIONS)return UMI_STATUS_CAPACITY_EXCEEDED;p->relocations[p->relocation_count++]=*r;return UMI_STATUS_OK;}
size_t umi_nc_object_plan_total_size(const UmiNativeObjectPlan *p){if(p==NULL)return 0U;size_t total=0U;for(size_t i=0U;i<p->section_count;i++){size_t n=umi_nc_object_section_aligned_size(&p->sections[i]);if(n>SIZE_MAX-total)return SIZE_MAX;total+=n;}return total;}
