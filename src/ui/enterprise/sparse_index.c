/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/sparse_index.c
 *
 * PURPOSE:
 *   Implement a bounded sorted sparse index.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/sparse_index.h"
#include <string.h>
void umi_ui_ent_sparse_index_init(UmiUiEntSparseIndex *x){if(x)memset(x,0,sizeof *x);}
UmiStatus umi_ui_ent_sparse_index_put(UmiUiEntSparseIndex *x,uint64_t key,size_t logical){size_t i,pos=0;if(!x||key==0U)return UMI_STATUS_INVALID_ARGUMENT;while(pos<x->count&&x->entries[pos].key<key)pos++;if(pos<x->count&&x->entries[pos].key==key){x->entries[pos].logical_index=logical;return UMI_STATUS_OK;}if(x->count>=UMI_UI_ENT_MAX_CACHE)return UMI_STATUS_CAPACITY_EXCEEDED;for(i=x->count;i>pos;i--)x->entries[i]=x->entries[i-1U];x->entries[pos]=(UmiUiEntSparseIndexEntry){key,logical};x->count++;return UMI_STATUS_OK;}
int umi_ui_ent_sparse_index_find(const UmiUiEntSparseIndex *x,uint64_t key,size_t *out){size_t lo=0,hi;if(!x||!out)return 0;hi=x->count;while(lo<hi){size_t mid=lo+(hi-lo)/2U;if(x->entries[mid].key==key){*out=x->entries[mid].logical_index;return 1;}if(x->entries[mid].key<key)lo=mid+1U;else hi=mid;}return 0;}
