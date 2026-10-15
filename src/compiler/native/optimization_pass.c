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
#include "../../base/value_construction_internal.h"
#include <string.h>
/*
 * Initialise nc optimization pass from caller-provided values so later operations receive
 * a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_nc_optimization_pass_init(UmiNativeOptimizationPass *p,const char *id,UmiNativeOptimizationPassKind kind,UmiNativeOptimizationLevel min,bool repeatable){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||id==NULL||id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(p->id,sizeof(p->id),id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;p->kind=kind;p->minimum_level=min;p->repeatable=repeatable;return UMI_STATUS_OK;}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_nc_optimization_pass_init_build_value(UmiNativeOptimizationPass *p,const char *id,UmiNativeOptimizationPassKind kind,UmiNativeOptimizationLevel min,bool repeatable){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||id==NULL||id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(p->id,sizeof(p->id),id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;p->kind=kind;p->minimum_level=min;p->repeatable=repeatable;return UMI_STATUS_OK;}

UMI_DEFINE_STAGED_VALUE_INIT(umi_nc_optimization_pass_init, UmiNativeOptimizationPass, p,
    (UmiNativeOptimizationPass *p, const char *id, UmiNativeOptimizationPassKind kind, UmiNativeOptimizationLevel min, bool repeatable),
    umi_nc_optimization_pass_init_build_value(candidate, id, kind, min, repeatable))

/*
 * Provide the nc optimization pass enabled operation used by this module and its client
 * applications.
 */
bool umi_nc_optimization_pass_enabled(const UmiNativeOptimizationPass *p,const UmiNativeOptimizationProfile *profile){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||profile==NULL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile->level==UMI_NC_OPT_SIZE)return p->minimum_level<=UMI_NC_OPT_O2;return profile->level>=p->minimum_level;}
