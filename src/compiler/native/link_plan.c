/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/link_plan.c
 *
 * PURPOSE:
 *   Build deterministic native link plans from objects, libraries, search roots and output kind.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/link_plan.h"
#include <string.h>
/*
 * Initialise nc link plan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_link_plan_init(UmiNativeLinkPlan *p,const char *out,UmiNativeLinkOutputKind k){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||out==NULL||out[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));p->kind=k;return umi_nc_copy_text(p->output,sizeof(p->output),out);}
/*
 * Provide the nc link plan add input operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_link_plan_add_input(UmiNativeLinkPlan *p,const char *path){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||path==NULL||path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->input_count>=UMI_NC_MAX_LINK_INPUTS)return UMI_STATUS_CAPACITY_EXCEEDED;UmiStatus st=umi_nc_copy_text(p->inputs[p->input_count],sizeof(p->inputs[0]),path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)p->input_count++;return st;}
/*
 * Provide the nc link plan add library operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_link_plan_add_library(UmiNativeLinkPlan *p,const char *lib){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||lib==NULL||lib[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->library_count>=UMI_NC_MAX_LINK_LIBRARIES)return UMI_STATUS_CAPACITY_EXCEEDED;UmiStatus st=umi_nc_copy_text(p->libraries[p->library_count],sizeof(p->libraries[0]),lib);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)p->library_count++;return st;}
/*
 * Provide the nc link plan fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_nc_link_plan_fingerprint(const UmiNativeLinkPlan *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return 0U;uint64_t h=umi_nc_hash_text(p->output)^(uint64_t)p->kind;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p->input_count;i++)h^=umi_nc_hash_text(p->inputs[i])+(h<<6U)+(h>>2U);/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p->library_count;i++)h^=umi_nc_hash_text(p->libraries[i])+(h<<6U)+(h>>2U);return h;}
