/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/diversity_policy.c
 *
 * PURPOSE:
 *   Measure and preserve implementation diversity to avoid repeated equivalent repair candidates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/diversity_policy.h"
/* Count differing fingerprint bits using portable integer operations. */
double umi_helix_diversity_policy_distance(uint64_t a,uint64_t b){ uint64_t x=a^b; unsigned c=0U; while(x!=0U){c+=(unsigned)(x&1U);x>>=1U;} return (double)c/64.0; }
/* Compare fingerprint distance with an explicit generation policy threshold. */
bool umi_helix_diversity_policy_accept(const UmiHelixDiversityPolicy *p,uint64_t a,uint64_t b){ return p!=NULL&&umi_helix_diversity_policy_distance(a,b)>=umi_helix_clamp_score(p->minimum_distance); }
