/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/object_plan.c
 *
 * PURPOSE:
 *   Aggregate object sections, relocations and object-format intent before backend serialization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/object_plan.h"
#include <string.h>
/*
 * Initialise nc object plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_object_plan_init(UmiNativeObjectPlan *p,UmiNativeObjectFormat f){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL){memset(p,0,sizeof(*p));p->format=f;}}
/*
 * Provide the nc object plan add section operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_object_plan_add_section(UmiNativeObjectPlan *p,const UmiNativeObjectSection *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->section_count>=UMI_NC_MAX_OBJECT_SECTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p->section_count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->sections[i].name,s->name)==0)return UMI_STATUS_ALREADY_EXISTS;p->sections[p->section_count++]=*s;return UMI_STATUS_OK;}
/*
 * Provide the nc object plan add relocation operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_object_plan_add_relocation(UmiNativeObjectPlan *p,const UmiNativeRelocation *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;UmiStatus st=umi_nc_relocation_validate(r,p->format);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->relocation_count>=UMI_NC_MAX_RELOCATIONS)return UMI_STATUS_CAPACITY_EXCEEDED;p->relocations[p->relocation_count++]=*r;return UMI_STATUS_OK;}
/*
 * Return the number of records represented by nc object plan total without changing their
 * state.
 */
size_t umi_nc_object_plan_total_size(const UmiNativeObjectPlan *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return 0U;size_t total=0U;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p->section_count;i++){size_t n=umi_nc_object_section_aligned_size(&p->sections[i]);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n>SIZE_MAX-total)return SIZE_MAX;total+=n;}return total;}
