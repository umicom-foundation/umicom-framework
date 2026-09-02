/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_autonomy_level.c
 *
 * PURPOSE:
 *   Regression coverage for classify manual, assisted, supervised and guarded-autonomous operating modes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/autonomy_level.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixAutonomyLevel r; umi_helix_autonomy_level_init(&r,"autonomy_level"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_autonomy_level_validate(&r)!=UMI_STATUS_OK){ return 1; } r.progress=0.75; double s=umi_helix_autonomy_level_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
