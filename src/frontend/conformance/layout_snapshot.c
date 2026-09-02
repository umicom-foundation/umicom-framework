/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/layout_snapshot.c
 *
 * PURPOSE:
 *   normalised layout snapshot state and deterministic layout fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/layout_snapshot.h"

/*
 * Initialise fc layout snapshot from caller-provided values so later operations receive a
 * known state.
 */
void umi_fc_layout_snapshot_init(UmiFcLayoutSnapshot *snapshot,const char *id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(snapshot!=NULL){*snapshot=(UmiFcLayoutSnapshot){0};/* Protect caller-owned memory by checking that required state is available before it is used. */ if(id!=NULL)(void)umi_fc_copy_text(snapshot->id,sizeof(snapshot->id),id);}}
/* Add fc layout snapshot only after its inputs and available capacity have been checked. */
UmiStatus umi_fc_layout_snapshot_append(UmiFcLayoutSnapshot *snapshot,double value){union{double d;uint64_t u;}bits;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(snapshot==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(snapshot->count>=UMI_FC_MAX_LAYOUT_VALUES)return UMI_STATUS_CAPACITY_EXCEEDED;snapshot->values[snapshot->count++]=value;bits.d=value;snapshot->fingerprint=umi_fc_hash_combine(snapshot->fingerprint,bits.u);snapshot->revision++;return UMI_STATUS_OK;}
/*
 * Provide the fc layout snapshot fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_layout_snapshot_fingerprint(UmiFcLayoutSnapshot *snapshot){size_t i;uint64_t h;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(snapshot==NULL)return 0U;h=umi_fc_hash_text(snapshot->id);/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<snapshot->count;++i){union{double d;uint64_t u;}b;b.d=snapshot->values[i];h=umi_fc_hash_combine(h,b.u);}snapshot->fingerprint=h;return h;}
