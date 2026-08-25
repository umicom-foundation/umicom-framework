/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_file_change.c
 *
 * PURPOSE:
 *   Regression coverage for represent a bounded file-level add, modify, delete or rename operation with content fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/file_change.h"
int main(void){ UmiHelixFileChange r; umi_helix_file_change_init(&r,"file_change"); if(umi_helix_file_change_validate(&r)!=UMI_STATUS_OK){ return 1; } r.risk=0.2; double s=umi_helix_file_change_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
