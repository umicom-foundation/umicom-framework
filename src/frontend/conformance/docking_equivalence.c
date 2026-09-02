/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/docking_equivalence.c
 *
 * PURPOSE:
 *   docking-feature parity and responsive fallback scoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/docking_equivalence.h"

/*
 * Provide the fc docking equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_docking_equivalence_evaluate(const UmiFcDockingEquivalence *item){unsigned req,hit;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->supported);return umi_fc_ratio((size_t)hit,(size_t)req);}
/*
 * Provide the fc docking equivalence outcome operation used by this module and its client
 * applications.
 */
UmiFcOutcome umi_fc_docking_equivalence_outcome(const UmiFcDockingEquivalence *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_FC_FAIL; /* Protect caller-owned memory by checking that required state is available before it is used. */ if((item->required&~item->supported)==0U)return UMI_FC_PASS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item->responsive_fallback)return UMI_FC_DEGRADED; return UMI_FC_FAIL;}
