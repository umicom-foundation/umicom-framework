/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/repair_ranking.h
 *
 * PURPOSE:
 *   Rank repair candidates deterministically by confidence, risk, validation cost and change size.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_REPAIR_RANKING_H
#define UMICOM_DEVELOPER_HELIX_REPAIR_RANKING_H
#include "umicom/developer/helix/repair_candidate.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixRepairRanking { UmiHelixRepairCandidate items[UMI_HELIX_MAX_ITEMS]; size_t count; } UmiHelixRepairRanking;
/* Add one validated repair candidate. */
UmiStatus umi_helix_repair_ranking_add(UmiHelixRepairRanking *ranking,const UmiHelixRepairCandidate *candidate);
/* Sort candidates best-first using deterministic score then identifier tie-breaking. */
void umi_helix_repair_ranking_sort(UmiHelixRepairRanking *ranking);
/* Return the highest-ranked candidate or NULL when no candidate exists. */
const UmiHelixRepairCandidate *umi_helix_repair_ranking_best(const UmiHelixRepairRanking *ranking);
#ifdef __cplusplus
}
#endif
#endif
