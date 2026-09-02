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

/*
 * Provide the fc focus equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_focus_equivalence_evaluate(const UmiFcFocusEquivalence *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;return umi_fc_ratio(item->actual_order,item->expected_order);}
/*
 * Provide the fc focus equivalence passes operation used by this module and its client
 * applications.
 */
bool umi_fc_focus_equivalence_passes(const UmiFcFocusEquivalence *item,double minimum){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||item->traps_missing>0U)return false;return umi_fc_focus_equivalence_evaluate(item)>=minimum;}
