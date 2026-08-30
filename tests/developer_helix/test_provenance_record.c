/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_provenance_record.c
 *
 * PURPOSE:
 *   Regression coverage for record source revision, model/tool/provider, policy and evidence fingerprints for generated changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/provenance_record.h"
int main(void){ UmiHelixProvenanceRecord r; umi_helix_provenance_record_init(&r,"provenance_record"); if(umi_helix_provenance_record_validate(&r)!=UMI_STATUS_OK){ return 1; } r.confidence=0.8; double s=umi_helix_provenance_record_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
