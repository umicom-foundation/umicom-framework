/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/coverage_summary.c
 *
 * PURPOSE:
 *   required/optional semantic capability coverage summary for a renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/coverage_summary.h"

/*
 * Provide the fc coverage summary calculate operation used by this module and its client
 * applications.
 */
void umi_fc_coverage_summary_calculate(uint64_t required,uint64_t optional,uint64_t actual,UmiFcCoverageSummary *out_summary){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_summary==NULL)return;out_summary->required_total=(size_t)__builtin_popcountll(required);out_summary->required_covered=(size_t)__builtin_popcountll(required&actual);out_summary->optional_total=(size_t)__builtin_popcountll(optional);out_summary->optional_covered=(size_t)__builtin_popcountll(optional&actual);out_summary->required_score=umi_fc_ratio(out_summary->required_covered,out_summary->required_total);out_summary->total_score=umi_fc_ratio(out_summary->required_covered+out_summary->optional_covered,out_summary->required_total+out_summary->optional_total);}
