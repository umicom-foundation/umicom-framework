/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_checkpoint.c
 *
 * PURPOSE:
 *   Regression coverage for represent a recoverable engineering checkpoint with repository, workspace and validation fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/checkpoint.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixCheckpoint r; umi_helix_checkpoint_init(&r,"checkpoint"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_checkpoint_validate(&r)!=UMI_STATUS_OK){ return 1; } r.confidence=0.8; double s=umi_helix_checkpoint_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
