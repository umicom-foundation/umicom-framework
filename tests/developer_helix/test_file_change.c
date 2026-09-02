/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_file_change.c
 *
 * PURPOSE:
 *   Regression coverage for represent a bounded file-level add, modify, delete or rename operation with content fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/file_change.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixFileChange r; umi_helix_file_change_init(&r,"file_change"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_file_change_validate(&r)!=UMI_STATUS_OK){ return 1; } r.risk=0.2; double s=umi_helix_file_change_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
