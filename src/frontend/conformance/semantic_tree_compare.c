/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/semantic_tree_compare.c
 *
 * PURPOSE:
 *   tree-shape, role and ordering comparison independent of native widget classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/semantic_tree_compare.h"

static size_t interact(const UmiFcSemanticTree *t){size_t i,n=0U;for(i=0U;t!=NULL&&i<t->count;++i)if(t->nodes[i].interactive)n++;return n;}
static double count_score(size_t a,size_t b){size_t hi=a>b?a:b,lo=a>b?b:a;return hi==0U?1.0:(double)lo/(double)hi;}
UmiStatus umi_fc_semantic_tree_compare_run(const UmiFcSemanticTree *expected,const UmiFcSemanticTree *actual,UmiFcSemanticTreeComparison *out_comparison){if(expected==NULL||actual==NULL||out_comparison==NULL)return UMI_STATUS_INVALID_ARGUMENT;out_comparison->count_score=count_score(expected->count,actual->count);out_comparison->shape_score=umi_fc_semantic_tree_shape_hash(expected)==umi_fc_semantic_tree_shape_hash(actual)?1.0:0.0;out_comparison->interactive_score=count_score(interact(expected),interact(actual));out_comparison->total_score=(out_comparison->count_score+out_comparison->shape_score+out_comparison->interactive_score)/3.0;return UMI_STATUS_OK;}
