/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/context_equivalence.c
 *
 * PURPOSE:
 *   context propagation parity and directionality scoring across frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/context_equivalence.h"

double umi_fc_context_equivalence_evaluate(const UmiFcContextEquivalence *item){unsigned req,hit;if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->supported);return umi_fc_ratio((size_t)hit,(size_t)req);}
UmiFcOutcome umi_fc_context_equivalence_outcome(const UmiFcContextEquivalence *item){if(item==NULL)return UMI_FC_FAIL; if((item->required&~item->supported)==0U && item->direction_mismatches==0U)return UMI_FC_PASS; if((item->required&~item->supported)==0U)return UMI_FC_DEGRADED; return UMI_FC_FAIL;}
