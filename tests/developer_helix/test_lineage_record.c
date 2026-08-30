/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_lineage_record.c
 *
 * PURPOSE:
 *   Regression coverage for record parent/child variant lineage and transformation reason for reproducible evolution history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/lineage_record.h"
int main(void){ UmiHelixLineageRecord r; umi_helix_lineage_record_init(&r,"lineage_record"); if(umi_helix_lineage_record_validate(&r)!=UMI_STATUS_OK){ return 1; } r.confidence=0.8; double s=umi_helix_lineage_record_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
