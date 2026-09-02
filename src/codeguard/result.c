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

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/result.h"
#include <stdlib.h>
#include <string.h>
struct UmiCodeGuardResult { UmiCodeGuardFinding *items; size_t count; size_t capacity; };
/*
 * Initialise codeguard result from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_codeguard_result_create(size_t initial_capacity,UmiCodeGuardResult **out_result)
{
    UmiCodeGuardResult *r;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(out_result==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_result=NULL;
    r=(UmiCodeGuardResult*)calloc(1U,sizeof(*r));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(r==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Apply this branch only when its contract condition is satisfied. */
    if(initial_capacity==0U) initial_capacity=64U;
    r->items=(UmiCodeGuardFinding*)calloc(initial_capacity,sizeof(*r->items));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(r->items==NULL){free(r);return UMI_STATUS_OUT_OF_MEMORY;}
    r->capacity=initial_capacity;*out_result=r;return UMI_STATUS_OK;
}
/*
 * Release or reset state held by codeguard result so the same storage can be reused
 * safely.
 */
void umi_codeguard_result_destroy(UmiCodeGuardResult *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r!=NULL){free(r->items);free(r);}}
/*
 * Release or reset state held by codeguard result so the same storage can be reused
 * safely.
 */
void umi_codeguard_result_clear(UmiCodeGuardResult *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r!=NULL)r->count=0U;}
/* Add codeguard result only after its inputs and available capacity have been checked. */
UmiStatus umi_codeguard_result_add(UmiCodeGuardResult *r,const UmiCodeGuardFinding *f)
{
    UmiCodeGuardFinding *items;size_t cap;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(r==NULL||f==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(r->count==r->capacity){cap=r->capacity<64U?64U:r->capacity*2U;items=(UmiCodeGuardFinding*)realloc(r->items,cap*sizeof(*items));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(items==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->items=items;r->capacity=cap;}
    r->items[r->count++]=*f;return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by codeguard result without changing their
 * state.
 */
size_t umi_codeguard_result_count(const UmiCodeGuardResult *r){return r!=NULL?r->count:0U;}
/*
 * Find codeguard result while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCodeGuardFinding *umi_codeguard_result_at(const UmiCodeGuardResult *r,size_t i){return r!=NULL&&i<r->count?&r->items[i]:NULL;}
/*
 * Provide the codeguard result count severity operation used by this module and its client
 * applications.
 */
size_t umi_codeguard_result_count_severity(const UmiCodeGuardResult *r,UmiCodeGuardSeverity s){size_t i,n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r!=NULL)/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->items[i].severity==s)++n;return n;}
/*
 * Provide the codeguard result count category operation used by this module and its client
 * applications.
 */
size_t umi_codeguard_result_count_category(const UmiCodeGuardResult *r,UmiCodeGuardCategory c){size_t i,n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r!=NULL)/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->items[i].category==c)++n;return n;}
