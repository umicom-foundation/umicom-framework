/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/context_equivalence.c
 *
 * PURPOSE:
 *   context propagation parity and directionality scoring across frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/context_equivalence.h"

/*
 * Provide the fc context equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_context_equivalence_evaluate(const UmiFcContextEquivalence *item){unsigned req,hit;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->supported);return umi_fc_ratio((size_t)hit,(size_t)req);}
/*
 * Provide the fc context equivalence outcome operation used by this module and its client
 * applications.
 */
UmiFcOutcome umi_fc_context_equivalence_outcome(const UmiFcContextEquivalence *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_FC_FAIL; /* Protect caller-owned memory by checking that required state is available before it is used. */ if((item->required&~item->supported)==0U && item->direction_mismatches==0U)return UMI_FC_PASS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if((item->required&~item->supported)==0U)return UMI_FC_DEGRADED; return UMI_FC_FAIL;}
