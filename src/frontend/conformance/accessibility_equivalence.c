/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/accessibility_equivalence.c
 *
 * PURPOSE:
 *   accessibility capability coverage and blocker detection across frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/accessibility_equivalence.h"

/*
 * Provide the fc accessibility equivalence evaluate operation used by this module and its
 * client applications.
 */
double umi_fc_accessibility_equivalence_evaluate(const UmiFcAccessibilityEquivalence *item){unsigned req,hit;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required&item->supported);return umi_fc_ratio((size_t)hit,(size_t)req);}
/*
 * Provide the fc accessibility equivalence has blocker operation used by this module and
 * its client applications.
 */
bool umi_fc_accessibility_equivalence_has_blocker(const UmiFcAccessibilityEquivalence *item){return item==NULL||((item->required&~item->supported)!=0U);}
