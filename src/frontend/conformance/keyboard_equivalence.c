/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/keyboard_equivalence.c
 *
 * PURPOSE:
 *   keyboard command coverage scoring and mandatory shortcut detection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/keyboard_equivalence.h"

double umi_fc_keyboard_equivalence_evaluate(const UmiFcKeyboardEquivalence *item){if(item==NULL)return 0.0;return umi_fc_ratio(item->implemented_commands,item->required_commands);}
bool umi_fc_keyboard_equivalence_passes(const UmiFcKeyboardEquivalence *item,double minimum){if(item==NULL||item->mandatory_missing>0U)return false;return umi_fc_keyboard_equivalence_evaluate(item)>=minimum;}
