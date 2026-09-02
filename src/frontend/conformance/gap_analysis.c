/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/gap_analysis.c
 *
 * PURPOSE:
 *   required, optional and quality gap analysis between canonical surface and renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/gap_analysis.h"

/*
 * Perform fc gap analysis through the module contract so client applications do not
 * duplicate its policy.
 */
void umi_fc_gap_analysis_run(uint64_t required,uint64_t optional,uint64_t actual,UmiFcGapAnalysis *out_analysis){unsigned total,hit;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_analysis==NULL)return;out_analysis->required_missing=required&~actual;out_analysis->optional_missing=optional&~actual;out_analysis->required_count=(size_t)__builtin_popcountll(out_analysis->required_missing);out_analysis->optional_count=(size_t)__builtin_popcountll(out_analysis->optional_missing);total=(unsigned)__builtin_popcountll(required|optional);hit=(unsigned)__builtin_popcountll((required|optional)&actual);out_analysis->coverage=umi_fc_ratio((size_t)hit,(size_t)total);}
