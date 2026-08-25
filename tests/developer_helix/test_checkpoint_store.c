/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_checkpoint_store.c
 *
 * PURPOSE:
 *   Regression coverage for maintain a bounded in-memory checkpoint catalogue and deterministic latest-checkpoint lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/checkpoint_store.h"
int main(void){ UmiHelixCheckpointStore s={0}; UmiHelixCheckpoint a,b; umi_helix_checkpoint_init(&a,"a"); a.confidence=1.0; a.sequence=1; umi_helix_checkpoint_init(&b,"b"); b.confidence=1.0; b.sequence=2; if(umi_helix_checkpoint_store_add(&s,&a)!=UMI_STATUS_OK||umi_helix_checkpoint_store_add(&s,&b)!=UMI_STATUS_OK){ return 1; } const UmiHelixCheckpoint *p=umi_helix_checkpoint_store_latest(&s); if(p==NULL||p->sequence!=2U){ return 2; } return 0; }
