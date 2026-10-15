/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/target_catalogue.c
 *
 * PURPOSE:
 *   Maintain a bounded catalogue of named cross-target profiles for deterministic selection.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include "umicom/platform/cross_target/target_catalogue.h"

#include <string.h>
/*
 * Initialise ct target catalogue from caller-provided values so later operations receive a
 * known state.
 */
void umi_ct_target_catalogue_init(UmiCtTargetCatalogue*c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!=NULL)memset(c,0,sizeof(*c));}
/* Add ct target catalogue only after its inputs and available capacity have been checked. */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_ct_target_catalogue_add(UmiCtTargetCatalogue*c,const UmiCtTargetProfile*p){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->items[i].profile_id,p->profile_id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*p;c->revision++;return UMI_STATUS_OK;}
#endif

UmiStatus umi_ct_target_catalogue_add(UmiCtTargetCatalogue*c,const UmiCtTargetProfile*p){
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (c != NULL && c->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->items[i].profile_id,p->profile_id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*p;c->revision++;return UMI_STATUS_OK;}
/*
 * Find ct target catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCtTargetProfile *umi_ct_target_catalogue_find(const UmiCtTargetCatalogue*c,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->items[i].profile_id,id)==0)return &c->items[i];return NULL;}
