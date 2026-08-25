/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/layout_snapshot.c
 *
 * PURPOSE:
 *   normalised layout snapshot state and deterministic layout fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/layout_snapshot.h"

void umi_fc_layout_snapshot_init(UmiFcLayoutSnapshot *snapshot,const char *id){if(snapshot!=NULL){*snapshot=(UmiFcLayoutSnapshot){0};if(id!=NULL)(void)umi_fc_copy_text(snapshot->id,sizeof(snapshot->id),id);}}
UmiStatus umi_fc_layout_snapshot_append(UmiFcLayoutSnapshot *snapshot,double value){union{double d;uint64_t u;}bits;if(snapshot==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(snapshot->count>=UMI_FC_MAX_LAYOUT_VALUES)return UMI_STATUS_CAPACITY_EXCEEDED;snapshot->values[snapshot->count++]=value;bits.d=value;snapshot->fingerprint=umi_fc_hash_combine(snapshot->fingerprint,bits.u);snapshot->revision++;return UMI_STATUS_OK;}
uint64_t umi_fc_layout_snapshot_fingerprint(UmiFcLayoutSnapshot *snapshot){size_t i;uint64_t h;if(snapshot==NULL)return 0U;h=umi_fc_hash_text(snapshot->id);for(i=0U;i<snapshot->count;++i){union{double d;uint64_t u;}b;b.d=snapshot->values[i];h=umi_fc_hash_combine(h,b.u);}snapshot->fingerprint=h;return h;}
