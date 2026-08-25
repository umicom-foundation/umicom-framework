/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/command_equivalence.c
 *
 * PURPOSE:
 *   command-surface parity scoring and missing-command blocker reporting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/command_equivalence.h"

double umi_fc_command_equivalence_evaluate(const UmiFcCommandEquivalence *item){if(item==NULL)return 0.0;return umi_fc_ratio(item->implemented_commands,item->required_commands);}
bool umi_fc_command_equivalence_passes(const UmiFcCommandEquivalence *item,double minimum){if(item==NULL||item->blockers>0U)return false;return umi_fc_command_equivalence_evaluate(item)>=minimum;}
