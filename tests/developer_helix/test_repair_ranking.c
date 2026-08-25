/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_repair_ranking.c
 *
 * PURPOSE:
 *   Regression coverage for rank repair candidates deterministically by confidence, risk, validation cost and change size.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/repair_ranking.h"
int main(void){ UmiHelixRepairRanking r={0}; UmiHelixRepairCandidate a,b; umi_helix_repair_candidate_init(&a,"a"); a.confidence=0.6; a.risk=0.2; a.cost=0.2; umi_helix_repair_candidate_init(&b,"b"); b.confidence=0.9; b.risk=0.1; b.cost=0.1; if(umi_helix_repair_ranking_add(&r,&a)!=UMI_STATUS_OK||umi_helix_repair_ranking_add(&r,&b)!=UMI_STATUS_OK){ return 1; } umi_helix_repair_ranking_sort(&r); if(umi_helix_repair_ranking_best(&r)!=&r.items[0]||r.items[0].id[0]!='b'){ return 2; } return 0; }
