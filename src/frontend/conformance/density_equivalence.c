/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/density_equivalence.c
 *
 * PURPOSE:
 *   density-token equivalence scoring between renderer implementations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/density_equivalence.h"

static double fc_abs(double v){return v<0.0?-v:v;}
double umi_fc_density_equivalence_evaluate(const UmiFcDensityEquivalence *item){double delta;if(item==NULL||item->tolerance<0.0)return 0.0;delta=fc_abs(item->expected-item->actual);if(delta<=item->tolerance)return 1.0;if(item->tolerance==0.0)return 0.0;return umi_fc_clamp_score(1.0-(delta-item->tolerance)/(item->tolerance*4.0));}
