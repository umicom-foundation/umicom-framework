/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/checkpoint_store.c
 *
 * PURPOSE:
 *   Maintain a bounded in-memory checkpoint catalogue and deterministic latest-checkpoint lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/checkpoint_store.h"
#include <string.h>
/* Append immutable checkpoint evidence to bounded session storage. */
UmiStatus umi_helix_checkpoint_store_add(UmiHelixCheckpointStore *s,const UmiHelixCheckpoint *c){ if(s==NULL||c==NULL||umi_helix_checkpoint_validate(c)!=UMI_STATUS_OK){ return UMI_STATUS_INVALID_ARGUMENT; } if(s->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } if(umi_helix_checkpoint_store_find(s,c->id)!=NULL){ return UMI_STATUS_ALREADY_EXISTS; } s->items[s->count++]=*c; return UMI_STATUS_OK; }
/* Find exact checkpoint identity without fallback guessing. */
const UmiHelixCheckpoint *umi_helix_checkpoint_store_find(const UmiHelixCheckpointStore *s,const char *id){ if(s==NULL||id==NULL){ return NULL; } for(size_t i=0;i<s->count;i++) if(strcmp(s->items[i].id,id)==0){ return &s->items[i]; } return NULL; }
/* Select the latest sequence deterministically. */
const UmiHelixCheckpoint *umi_helix_checkpoint_store_latest(const UmiHelixCheckpointStore *s){ if(s==NULL||s->count==0U){ return NULL; } size_t best=0U; for(size_t i=1;i<s->count;i++) if(s->items[i].sequence>s->items[best].sequence) best=i; return &s->items[best]; }
