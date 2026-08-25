/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/repair_ranking.c
 *
 * PURPOSE:
 *   Rank repair candidates deterministically by confidence, risk, validation cost and change size.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/repair_ranking.h"
#include <string.h>
/* Copy a structurally valid repair candidate into bounded ranking storage. */
UmiStatus umi_helix_repair_ranking_add(UmiHelixRepairRanking *r,const UmiHelixRepairCandidate *c){ if(r==NULL||c==NULL||umi_helix_repair_candidate_validate(c)!=UMI_STATUS_OK){ return UMI_STATUS_INVALID_ARGUMENT; } if(r->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } r->items[r->count++]=*c; return UMI_STATUS_OK; }
/* Order by descending evidence score with stable ID tie-breaking. */
void umi_helix_repair_ranking_sort(UmiHelixRepairRanking *r){ if(r==NULL){ return; } for(size_t i=1;i<r->count;i++){ UmiHelixRepairCandidate key=r->items[i]; size_t j=i; while(j>0){ double a=umi_helix_repair_candidate_score(&r->items[j-1]), b=umi_helix_repair_candidate_score(&key); bool move=(a<b)||(a==b&&strcmp(r->items[j-1].id,key.id)>0); if(!move) break; r->items[j]=r->items[j-1]; j--; } r->items[j]=key; } }
/* Expose the selected proposal without applying it. */
const UmiHelixRepairCandidate *umi_helix_repair_ranking_best(const UmiHelixRepairRanking *r){ return r==NULL||r->count==0U?NULL:&r->items[0]; }
