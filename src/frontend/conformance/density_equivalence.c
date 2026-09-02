/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/density_equivalence.c
 *
 * PURPOSE:
 *   density-token equivalence scoring between renderer implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/density_equivalence.h"

/* Provide the fc abs operation used by this module and its client applications. */
static double fc_abs(double v){return v<0.0?-v:v;}
/*
 * Provide the fc density equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_density_equivalence_evaluate(const UmiFcDensityEquivalence *item){double delta;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||item->tolerance<0.0)return 0.0;delta=fc_abs(item->expected-item->actual);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(delta<=item->tolerance)return 1.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item->tolerance==0.0)return 0.0;return umi_fc_clamp_score(1.0-(delta-item->tolerance)/(item->tolerance*4.0));}
