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

#include "umicom/platform/cross_target/target_catalogue.h"

#include <string.h>
/*
 * Initialise ct target catalogue from caller-provided values so later operations receive a
 * known state.
 */
void umi_ct_target_catalogue_init(UmiCtTargetCatalogue*c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!=NULL)memset(c,0,sizeof(*c));}
/* Add ct target catalogue only after its inputs and available capacity have been checked. */
UmiStatus umi_ct_target_catalogue_add(UmiCtTargetCatalogue*c,const UmiCtTargetProfile*p){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->items[i].profile_id,p->profile_id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*p;c->revision++;return UMI_STATUS_OK;}
/*
 * Find ct target catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCtTargetProfile *umi_ct_target_catalogue_find(const UmiCtTargetCatalogue*c,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->items[i].profile_id,id)==0)return &c->items[i];return NULL;}
