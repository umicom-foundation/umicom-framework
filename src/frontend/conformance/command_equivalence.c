/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/command_equivalence.c
 *
 * PURPOSE:
 *   command-surface parity scoring and missing-command blocker reporting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/command_equivalence.h"

/*
 * Provide the fc command equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_command_equivalence_evaluate(const UmiFcCommandEquivalence *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;return umi_fc_ratio(item->implemented_commands,item->required_commands);}
/*
 * Provide the fc command equivalence passes operation used by this module and its client
 * applications.
 */
bool umi_fc_command_equivalence_passes(const UmiFcCommandEquivalence *item,double minimum){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||item->blockers>0U)return false;return umi_fc_command_equivalence_evaluate(item)>=minimum;}
