/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_workspace_fingerprint.c
 *
 * PURPOSE:
 *   Regression coverage for combine repository, build, configuration and toolchain evidence into a deterministic workspace fingerprint.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/workspace_fingerprint.h"
int main(void){ UmiHelixWorkspaceFingerprint r; umi_helix_workspace_fingerprint_init(&r,"workspace_fingerprint"); if(umi_helix_workspace_fingerprint_validate(&r)!=UMI_STATUS_OK){ return 1; } r.confidence=0.8; double s=umi_helix_workspace_fingerprint_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
