/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_repository_baseline.c
 *
 * PURPOSE:
 *   Regression coverage for capture the trusted repository revision, dirty-state and lock evidence used as the engineering baseline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/repository_baseline.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixRepositoryBaseline r; umi_helix_repository_baseline_init(&r,"repository_baseline"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_repository_baseline_validate(&r)!=UMI_STATUS_OK){ return 1; } r.confidence=0.8; double s=umi_helix_repository_baseline_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
