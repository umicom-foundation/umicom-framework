/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/checkpoint_store.c
 *
 * PURPOSE:
 *   Maintain a bounded in-memory checkpoint catalogue and deterministic latest-checkpoint lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/checkpoint_store.h"
#include <string.h>
/* Append immutable checkpoint evidence to bounded session storage. */
UmiStatus umi_helix_checkpoint_store_add(UmiHelixCheckpointStore *s,const UmiHelixCheckpoint *c){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||c==NULL||umi_helix_checkpoint_validate(c)!=UMI_STATUS_OK){ return UMI_STATUS_INVALID_ARGUMENT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_helix_checkpoint_store_find(s,c->id)!=NULL){ return UMI_STATUS_ALREADY_EXISTS; } s->items[s->count++]=*c; return UMI_STATUS_OK; }
/* Find exact checkpoint identity without fallback guessing. */
const UmiHelixCheckpoint *umi_helix_checkpoint_store_find(const UmiHelixCheckpointStore *s,const char *id){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL){ return NULL; } /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<s->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->items[i].id,id)==0){ return &s->items[i]; } return NULL; }
/* Select the latest sequence deterministically. */
const UmiHelixCheckpoint *umi_helix_checkpoint_store_latest(const UmiHelixCheckpointStore *s){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->count==0U){ return NULL; } size_t best=0U; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=1;i<s->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->items[i].sequence>s->items[best].sequence) best=i; return &s->items[best]; }
