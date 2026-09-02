/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/selection_equivalence.c
 *
 * PURPOSE:
 *   selection-mode parity scoring and graceful-degradation classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/selection_equivalence.h"

/*
 * Provide the fc selection equivalence evaluate operation used by this module and its
 * client applications.
 */
double umi_fc_selection_equivalence_evaluate(const UmiFcSelectionEquivalence *item){unsigned req,hit;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->supported);return umi_fc_ratio((size_t)hit,(size_t)req);}
/*
 * Provide the fc selection equivalence outcome operation used by this module and its
 * client applications.
 */
UmiFcOutcome umi_fc_selection_equivalence_outcome(const UmiFcSelectionEquivalence *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_FC_FAIL; /* Protect caller-owned memory by checking that required state is available before it is used. */ if((item->required&~item->supported)==0U)return UMI_FC_PASS; return UMI_FC_FAIL;}
