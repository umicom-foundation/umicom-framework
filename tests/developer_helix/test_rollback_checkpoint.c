/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_rollback_checkpoint.c
 *
 * PURPOSE:
 *   Regression coverage for capture the exact pre-change evidence required to restore a governed engineering session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/rollback_checkpoint.h"
int main(void){ UmiHelixRollbackCheckpoint r; umi_helix_rollback_checkpoint_init(&r,"rollback_checkpoint"); if(umi_helix_rollback_checkpoint_validate(&r)!=UMI_STATUS_OK){ return 1; } r.confidence=0.8; double s=umi_helix_rollback_checkpoint_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
