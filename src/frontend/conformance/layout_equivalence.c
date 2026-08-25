/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/layout_equivalence.c
 *
 * PURPOSE:
 *   structural and geometric layout-equivalence scoring with configurable tolerance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/layout_equivalence.h"

static double fc_abs(double v){return v<0.0?-v:v;}
double umi_fc_layout_equivalence_values(const double *expected,const double *actual,size_t count,double tolerance){size_t i,ok=0U;if(count==0U)return 1.0;if(expected==NULL||actual==NULL||tolerance<0.0)return 0.0;for(i=0U;i<count;++i)if(fc_abs(expected[i]-actual[i])<=tolerance)ok++;return umi_fc_ratio(ok,count);}
bool umi_fc_layout_equivalence_passes(double score,double minimum){return umi_fc_clamp_score(score)>=umi_fc_clamp_score(minimum);}
