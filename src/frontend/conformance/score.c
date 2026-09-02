/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/score.c
 *
 * PURPOSE:
 *   weighted percentage scoring with blocker-aware grade classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/score.h"

/* Provide the fc score weighted operation used by this module and its client applications. */
double umi_fc_score_weighted(double passed_weight,double total_weight){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(total_weight<=0.0)return passed_weight<=0.0?1.0:0.0;return umi_fc_clamp_score(passed_weight/total_weight);}
/* Provide the fc score grade operation used by this module and its client applications. */
UmiFcGrade umi_fc_score_grade(double score,size_t blockers){score=umi_fc_clamp_score(score);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(blockers>0U||score<0.75)return UMI_FC_GRADE_FAIL;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(score>=0.95)return UMI_FC_GRADE_A;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(score>=0.88)return UMI_FC_GRADE_B;return UMI_FC_GRADE_C;}
