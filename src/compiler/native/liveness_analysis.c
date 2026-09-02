/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/liveness_analysis.c
 *
 * PURPOSE:
 *   Summarize per-block live-in value pressure for later register allocation work.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/liveness_analysis.h"
#include <string.h>
/* Provide the seen operation used by this module and its client applications. */
static bool seen(const uint32_t *ids,size_t n,uint32_t id){/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<n;i++)/* Apply this branch only when its contract condition is satisfied. */ if(ids[i]==id)return true;return false;}
/*
 * Provide the nc liveness analyze operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_liveness_analyze(const UmiNativeIrFunction *f,UmiNativeLivenessAnalysis *a){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||a==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(a,0,sizeof(*a));/* Visit each bounded item once so every record receives the same rule. */ for(size_t bi=0U;bi<f->block_count;bi++){const UmiNativeIrBlock *b=&f->blocks[bi];UmiNativeBlockLiveness *o=&a->blocks[a->block_count++];o->block_id=b->id;uint32_t defs[UMI_NC_MAX_IR_INSTRUCTIONS];size_t dn=0U;uint32_t uses[UMI_NC_MAX_IR_INSTRUCTIONS*UMI_NC_MAX_IR_OPERANDS];size_t un=0U;/* Visit each bounded item once so every record receives the same rule. */ for(size_t ii=0U;ii<b->instruction_count;ii++){const UmiNativeIrInstruction *in=&b->instructions[ii];/* Visit each bounded item once so every record receives the same rule. */ for(size_t j=0U;j<in->operand_count;j++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!seen(uses,un,in->operands[j])&&un<sizeof(uses)/sizeof(uses[0]))uses[un++]=in->operands[j];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(in->result_id!=0U&&dn<UMI_NC_MAX_IR_INSTRUCTIONS)defs[dn++]=in->result_id;}o->defined_count=dn;o->used_count=un;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<un;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!seen(defs,dn,uses[i]))o->live_in_count++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o->live_in_count>a->peak_live_in)a->peak_live_in=o->live_in_count;}return UMI_STATUS_OK;}
