/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/semantic_tree_compare.c
 *
 * PURPOSE:
 *   tree-shape, role and ordering comparison independent of native widget classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/semantic_tree_compare.h"

/* Provide the interact operation used by this module and its client applications. */
static size_t interact(const UmiFcSemanticTree *t){size_t i,n=0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;t!=NULL&&i<t->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->nodes[i].interactive)n++;return n;}
/* Provide the count score operation used by this module and its client applications. */
static double count_score(size_t a,size_t b){size_t hi=a>b?a:b,lo=a>b?b:a;return hi==0U?1.0:(double)lo/(double)hi;}
/*
 * Perform fc semantic tree compare through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_fc_semantic_tree_compare_run(const UmiFcSemanticTree *expected,const UmiFcSemanticTree *actual,UmiFcSemanticTreeComparison *out_comparison){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(expected==NULL||actual==NULL||out_comparison==NULL)return UMI_STATUS_INVALID_ARGUMENT;out_comparison->count_score=count_score(expected->count,actual->count);out_comparison->shape_score=umi_fc_semantic_tree_shape_hash(expected)==umi_fc_semantic_tree_shape_hash(actual)?1.0:0.0;out_comparison->interactive_score=count_score(interact(expected),interact(actual));out_comparison->total_score=(out_comparison->count_score+out_comparison->shape_score+out_comparison->interactive_score)/3.0;return UMI_STATUS_OK;}
