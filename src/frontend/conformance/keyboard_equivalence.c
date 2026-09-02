/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/keyboard_equivalence.c
 *
 * PURPOSE:
 *   keyboard command coverage scoring and mandatory shortcut detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/keyboard_equivalence.h"

/*
 * Provide the fc keyboard equivalence evaluate operation used by this module and its
 * client applications.
 */
double umi_fc_keyboard_equivalence_evaluate(const UmiFcKeyboardEquivalence *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;return umi_fc_ratio(item->implemented_commands,item->required_commands);}
/*
 * Provide the fc keyboard equivalence passes operation used by this module and its client
 * applications.
 */
bool umi_fc_keyboard_equivalence_passes(const UmiFcKeyboardEquivalence *item,double minimum){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||item->mandatory_missing>0U)return false;return umi_fc_keyboard_equivalence_evaluate(item)>=minimum;}
