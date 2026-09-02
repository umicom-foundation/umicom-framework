/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/snapshot_compare.c
 *
 * PURPOSE:
 *   semantic render snapshot comparison with tolerant volatile-field handling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/snapshot_compare.h"

/* Provide the count score operation used by this module and its client applications. */
static double count_score(size_t a,size_t b){size_t hi=a>b?a:b,lo=a>b?b:a;return hi==0U?1.0:(double)lo/(double)hi;}
/*
 * Perform fc snapshot compare through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_fc_snapshot_compare_run(const UmiFcRenderSnapshot *expected,const UmiFcRenderSnapshot *actual,UmiFcSnapshotComparison *out_comparison){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(expected==NULL||actual==NULL||out_comparison==NULL)return UMI_STATUS_INVALID_ARGUMENT;out_comparison->node_score=count_score(expected->node_count,actual->node_count);out_comparison->text_score=count_score(expected->text_bytes,actual->text_bytes);out_comparison->interaction_score=count_score(expected->interactive_count,actual->interactive_count);out_comparison->fingerprint_match=expected->semantic_fingerprint==actual->semantic_fingerprint;out_comparison->total_score=(out_comparison->node_score+out_comparison->text_score+out_comparison->interaction_score+(out_comparison->fingerprint_match?1.0:0.0))/4.0;return UMI_STATUS_OK;}
