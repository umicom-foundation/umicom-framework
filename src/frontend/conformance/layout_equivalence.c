/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/layout_equivalence.c
 *
 * PURPOSE:
 *   structural and geometric layout-equivalence scoring with configurable tolerance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/layout_equivalence.h"

/* Provide the fc abs operation used by this module and its client applications. */
static double fc_abs(double v){return v<0.0?-v:v;}
/*
 * Provide the fc layout equivalence values operation used by this module and its client
 * applications.
 */
double umi_fc_layout_equivalence_values(const double *expected,const double *actual,size_t count,double tolerance){size_t i,ok=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(count==0U)return 1.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(expected==NULL||actual==NULL||tolerance<0.0)return 0.0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(fc_abs(expected[i]-actual[i])<=tolerance)ok++;return umi_fc_ratio(ok,count);}
/*
 * Provide the fc layout equivalence passes operation used by this module and its client
 * applications.
 */
bool umi_fc_layout_equivalence_passes(double score,double minimum){return umi_fc_clamp_score(score)>=umi_fc_clamp_score(minimum);}
