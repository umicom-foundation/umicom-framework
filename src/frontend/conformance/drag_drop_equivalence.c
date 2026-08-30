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

double umi_fc_drag_drop_equivalence_evaluate(const UmiFcDragDropEquivalence *item){unsigned req,hit;if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->supported);return umi_fc_ratio((size_t)hit,(size_t)req);}
UmiFcOutcome umi_fc_drag_drop_equivalence_outcome(const UmiFcDragDropEquivalence *item){if(item==NULL)return UMI_FC_FAIL; if((item->required&~item->supported)==0U)return UMI_FC_PASS; if(item->keyboard_fallback)return UMI_FC_DEGRADED; return UMI_FC_FAIL;}
