/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/docking_equivalence.c
 *
 * PURPOSE:
 *   docking-feature parity and responsive fallback scoring.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/docking_equivalence.h"

double umi_fc_docking_equivalence_evaluate(const UmiFcDockingEquivalence *item){unsigned req,hit;if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->supported);return umi_fc_ratio((size_t)hit,(size_t)req);}
UmiFcOutcome umi_fc_docking_equivalence_outcome(const UmiFcDockingEquivalence *item){if(item==NULL)return UMI_FC_FAIL; if((item->required&~item->supported)==0U)return UMI_FC_PASS; if(item->responsive_fallback)return UMI_FC_DEGRADED; return UMI_FC_FAIL;}
