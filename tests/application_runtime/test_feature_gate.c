/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_feature_gate.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
/*
 * Exercise test feature gate and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_feature_gate(void){ const UmiApplicationExperienceDefinition *e=test_trader_experience(); UmiApplicationFeatureGateResult r; assert(umi_application_feature_gate_evaluate(e,e->features[0].feature_id,UMI_EXPERIENCE_FEATURE_PLANNED,test_capability_probe,NULL,&r)==UMI_STATUS_OK); assert(r.state_satisfied); return 0; }
