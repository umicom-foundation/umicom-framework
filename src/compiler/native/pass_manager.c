/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/pass_manager.c
 *
 * PURPOSE:
 *   Build deterministic optimization pipelines and filter passes by optimization profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/pass_manager.h"
#include <string.h>
void umi_nc_pass_manager_init(UmiNativePassManager *m){if(m!=NULL)memset(m,0,sizeof(*m));}
UmiStatus umi_nc_pass_manager_add(UmiNativePassManager *m,const UmiNativeOptimizationPass *p){if(m==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(m->count>=UMI_NC_MAX_OPT_PASSES)return UMI_STATUS_CAPACITY_EXCEEDED;for(size_t i=0U;i<m->count;i++)if(strcmp(m->passes[i].id,p->id)==0)return UMI_STATUS_ALREADY_EXISTS;m->passes[m->count++]=*p;return UMI_STATUS_OK;}
UmiStatus umi_nc_pass_manager_default(UmiNativePassManager *m){if(m==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_nc_pass_manager_init(m);UmiNativeOptimizationPass p;UmiStatus st=umi_nc_optimization_pass_init(&p,"constant-fold",UMI_NC_PASS_CONSTANT_FOLD,UMI_NC_OPT_O1,true);if(st!=UMI_STATUS_OK)return st;if(umi_nc_pass_manager_add(m,&p)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(umi_nc_optimization_pass_init(&p,"copy-prop",UMI_NC_PASS_COPY_PROP,UMI_NC_OPT_O1,true)!=UMI_STATUS_OK||umi_nc_pass_manager_add(m,&p)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(umi_nc_optimization_pass_init(&p,"branch-simplify",UMI_NC_PASS_BRANCH_SIMPLIFY,UMI_NC_OPT_O1,true)!=UMI_STATUS_OK||umi_nc_pass_manager_add(m,&p)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(umi_nc_optimization_pass_init(&p,"dce",UMI_NC_PASS_DCE,UMI_NC_OPT_O1,true)!=UMI_STATUS_OK||umi_nc_pass_manager_add(m,&p)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;return UMI_STATUS_OK;}
size_t umi_nc_pass_manager_enabled_count(const UmiNativePassManager *m,const UmiNativeOptimizationProfile *p){if(m==NULL||p==NULL)return 0U;size_t n=0U;for(size_t i=0U;i<m->count;i++)if(umi_nc_optimization_pass_enabled(&m->passes[i],p))n++;return n;}
