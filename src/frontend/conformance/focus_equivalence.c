/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/focus_equivalence.c
 *
 * PURPOSE:
 *   focus traversal equivalence scoring across native and browser renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/focus_equivalence.h"

double umi_fc_focus_equivalence_evaluate(const UmiFcFocusEquivalence *item){if(item==NULL)return 0.0;return umi_fc_ratio(item->actual_order,item->expected_order);}
bool umi_fc_focus_equivalence_passes(const UmiFcFocusEquivalence *item,double minimum){if(item==NULL||item->traps_missing>0U)return false;return umi_fc_focus_equivalence_evaluate(item)>=minimum;}
