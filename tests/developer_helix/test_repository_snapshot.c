/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_repository_snapshot.c
 *
 * PURPOSE:
 *   Regression coverage for capture bounded repository health/change evidence at one point in a helix session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/repository_snapshot.h"
int main(void){ UmiHelixRepositorySnapshot r; umi_helix_repository_snapshot_init(&r,"repository_snapshot"); if(umi_helix_repository_snapshot_validate(&r)!=UMI_STATUS_OK){ return 1; } r.risk=0.2; double s=umi_helix_repository_snapshot_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
