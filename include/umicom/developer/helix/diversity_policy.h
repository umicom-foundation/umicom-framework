/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/diversity_policy.h
 *
 * PURPOSE:
 *   Measure and preserve implementation diversity to avoid repeated equivalent repair candidates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_DIVERSITY_POLICY_H
#define UMICOM_DEVELOPER_HELIX_DIVERSITY_POLICY_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixDiversityPolicy { double minimum_distance; } UmiHelixDiversityPolicy;
/* Estimate normalized bit distance between two deterministic variant fingerprints. */
double umi_helix_diversity_policy_distance(uint64_t left,uint64_t right);
/* Report whether two variants satisfy the configured diversity floor. */
bool umi_helix_diversity_policy_accept(const UmiHelixDiversityPolicy *policy,uint64_t left,uint64_t right);
#ifdef __cplusplus
}
#endif
#endif
