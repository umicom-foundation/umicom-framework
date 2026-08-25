/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/event_equivalence.c
 *
 * PURPOSE:
 *   event-family parity scoring for pointer, keyboard, input, focus and drag/drop.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/event_equivalence.h"

double umi_fc_event_equivalence_evaluate(const UmiFcEventEquivalence *item){unsigned req,hit;if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->supported);return umi_fc_ratio((size_t)hit,(size_t)req);}
UmiFcOutcome umi_fc_event_equivalence_outcome(const UmiFcEventEquivalence *item){if(item==NULL)return UMI_FC_FAIL; if((item->required&~item->supported)==0U)return UMI_FC_PASS; return UMI_FC_FAIL;}
