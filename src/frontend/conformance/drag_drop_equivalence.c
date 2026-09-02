/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/drag_drop_equivalence.c
 *
 * PURPOSE:
 *   drag/drop parity scoring with keyboard-accessible fallback requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/drag_drop_equivalence.h"

/*
 * Provide the fc drag drop equivalence evaluate operation used by this module and its
 * client applications.
 */
double umi_fc_drag_drop_equivalence_evaluate(const UmiFcDragDropEquivalence *item){unsigned req,hit;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->supported);return umi_fc_ratio((size_t)hit,(size_t)req);}
/*
 * Provide the fc drag drop equivalence outcome operation used by this module and its
 * client applications.
 */
UmiFcOutcome umi_fc_drag_drop_equivalence_outcome(const UmiFcDragDropEquivalence *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_FC_FAIL; /* Protect caller-owned memory by checking that required state is available before it is used. */ if((item->required&~item->supported)==0U)return UMI_FC_PASS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item->keyboard_fallback)return UMI_FC_DEGRADED; return UMI_FC_FAIL;}
