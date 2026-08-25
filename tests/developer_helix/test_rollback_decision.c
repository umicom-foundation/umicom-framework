/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_rollback_decision.c
 *
 * PURPOSE:
 *   Regression coverage for evaluate validation, safety, build and test evidence to choose continue, stop or rollback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/rollback_decision.h"
int main(void){ if(umi_helix_rollback_decision_evaluate(false,false,false,0.01,0.05)!=UMI_HELIX_DECISION_ALLOW){ return 1; } if(umi_helix_rollback_decision_evaluate(false,true,false,0.0,0.05)!=UMI_HELIX_DECISION_ROLLBACK){ return 2; } return 0; }
