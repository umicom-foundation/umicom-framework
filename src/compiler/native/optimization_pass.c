/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/optimization_pass.c
 *
 * PURPOSE:
 *   Describe deterministic optimization pass metadata and minimum optimization levels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/optimization_pass.h"
#include <string.h>
/*
 * Initialise nc optimization pass from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_nc_optimization_pass_init(UmiNativeOptimizationPass *p,const char *id,UmiNativeOptimizationPassKind kind,UmiNativeOptimizationLevel min,bool repeatable){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||id==NULL||id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(p->id,sizeof(p->id),id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;p->kind=kind;p->minimum_level=min;p->repeatable=repeatable;return UMI_STATUS_OK;}
/*
 * Provide the nc optimization pass enabled operation used by this module and its client
 * applications.
 */
bool umi_nc_optimization_pass_enabled(const UmiNativeOptimizationPass *p,const UmiNativeOptimizationProfile *profile){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||profile==NULL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile->level==UMI_NC_OPT_SIZE)return p->minimum_level<=UMI_NC_OPT_O2;return profile->level>=p->minimum_level;}
