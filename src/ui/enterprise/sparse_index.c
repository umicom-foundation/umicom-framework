/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/sparse_index.c
 *
 * PURPOSE:
 *   Implement a bounded sorted sparse index.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/sparse_index.h"
#include <string.h>
/*
 * Initialise ui ent sparse index from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_sparse_index_init(UmiUiEntSparseIndex *x){/* Apply this branch only when its contract condition is satisfied. */ if(x)memset(x,0,sizeof *x);}
/*
 * Provide the ui ent sparse index put operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_sparse_index_put(UmiUiEntSparseIndex *x,uint64_t key,size_t logical){size_t i,pos=0;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!x||key==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(pos<x->count&&x->entries[pos].key<key)pos++;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(pos<x->count&&x->entries[pos].key==key){x->entries[pos].logical_index=logical;return UMI_STATUS_OK;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(x->count>=UMI_UI_ENT_MAX_CACHE)return UMI_STATUS_CAPACITY_EXCEEDED;/* Visit each bounded item once so every record receives the same rule. */ for(i=x->count;i>pos;i--)x->entries[i]=x->entries[i-1U];x->entries[pos]=(UmiUiEntSparseIndexEntry){key,logical};x->count++;return UMI_STATUS_OK;}
/*
 * Find ui ent sparse index while leaving the underlying catalogue or model owned by this
 * module.
 */
int umi_ui_ent_sparse_index_find(const UmiUiEntSparseIndex *x,uint64_t key,size_t *out){size_t lo=0,hi;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!x||!out)return 0;hi=x->count;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(lo<hi){size_t mid=lo+(hi-lo)/2U;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(x->entries[mid].key==key){*out=x->entries[mid].logical_index;return 1;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(x->entries[mid].key<key)lo=mid+1U;/* Use this fallback path when the earlier condition does not apply. */ else hi=mid;}return 0;}
