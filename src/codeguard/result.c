/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/result.c
 *
 * PURPOSE:
 *   Store an arbitrary number of findings while keeping ownership inside
 *   CodeGuard.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/result.h"
#include <stdlib.h>
#include <string.h>
struct UmiCodeGuardResult { UmiCodeGuardFinding *items; size_t count; size_t capacity; };
UmiStatus umi_codeguard_result_create(size_t initial_capacity,UmiCodeGuardResult **out_result)
{
    UmiCodeGuardResult *r;
    if(out_result==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_result=NULL;
    r=(UmiCodeGuardResult*)calloc(1U,sizeof(*r));
    if(r==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if(initial_capacity==0U) initial_capacity=64U;
    r->items=(UmiCodeGuardFinding*)calloc(initial_capacity,sizeof(*r->items));
    if(r->items==NULL){free(r);return UMI_STATUS_OUT_OF_MEMORY;}
    r->capacity=initial_capacity;*out_result=r;return UMI_STATUS_OK;
}
void umi_codeguard_result_destroy(UmiCodeGuardResult *r){if(r!=NULL){free(r->items);free(r);}}
void umi_codeguard_result_clear(UmiCodeGuardResult *r){if(r!=NULL)r->count=0U;}
UmiStatus umi_codeguard_result_add(UmiCodeGuardResult *r,const UmiCodeGuardFinding *f)
{
    UmiCodeGuardFinding *items;size_t cap;
    if(r==NULL||f==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(r->count==r->capacity){cap=r->capacity<64U?64U:r->capacity*2U;items=(UmiCodeGuardFinding*)realloc(r->items,cap*sizeof(*items));if(items==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->items=items;r->capacity=cap;}
    r->items[r->count++]=*f;return UMI_STATUS_OK;
}
size_t umi_codeguard_result_count(const UmiCodeGuardResult *r){return r!=NULL?r->count:0U;}
const UmiCodeGuardFinding *umi_codeguard_result_at(const UmiCodeGuardResult *r,size_t i){return r!=NULL&&i<r->count?&r->items[i]:NULL;}
size_t umi_codeguard_result_count_severity(const UmiCodeGuardResult *r,UmiCodeGuardSeverity s){size_t i,n=0U;if(r!=NULL)for(i=0U;i<r->count;++i)if(r->items[i].severity==s)++n;return n;}
size_t umi_codeguard_result_count_category(const UmiCodeGuardResult *r,UmiCodeGuardCategory c){size_t i,n=0U;if(r!=NULL)for(i=0U;i<r->count;++i)if(r->items[i].category==c)++n;return n;}
