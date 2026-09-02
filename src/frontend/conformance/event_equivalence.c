/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/event_equivalence.c
 *
 * PURPOSE:
 *   event-family parity scoring for pointer, keyboard, input, focus and drag/drop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/event_equivalence.h"

/*
 * Provide the fc event equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_event_equivalence_evaluate(const UmiFcEventEquivalence *item){unsigned req,hit;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->supported);return umi_fc_ratio((size_t)hit,(size_t)req);}
/*
 * Provide the fc event equivalence outcome operation used by this module and its client
 * applications.
 */
UmiFcOutcome umi_fc_event_equivalence_outcome(const UmiFcEventEquivalence *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_FC_FAIL; /* Protect caller-owned memory by checking that required state is available before it is used. */ if((item->required&~item->supported)==0U)return UMI_FC_PASS; return UMI_FC_FAIL;}
