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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/target_catalogue.h"

#include <string.h>
void umi_ct_target_catalogue_init(UmiCtTargetCatalogue*c){if(c!=NULL)memset(c,0,sizeof(*c));}
UmiStatus umi_ct_target_catalogue_add(UmiCtTargetCatalogue*c,const UmiCtTargetProfile*p){size_t i;if(c==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<c->count;++i)if(strcmp(c->items[i].profile_id,p->profile_id)==0)return UMI_STATUS_ALREADY_EXISTS;if(c->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*p;c->revision++;return UMI_STATUS_OK;}
const UmiCtTargetProfile *umi_ct_target_catalogue_find(const UmiCtTargetCatalogue*c,const char*id){size_t i;if(c==NULL||id==NULL)return NULL;for(i=0U;i<c->count;++i)if(strcmp(c->items[i].profile_id,id)==0)return &c->items[i];return NULL;}
