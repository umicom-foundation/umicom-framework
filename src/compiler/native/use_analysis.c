/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/use_analysis.c
 *
 * PURPOSE:
 *   Count IR value uses to support dead-code elimination and copy propagation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/use_analysis.h"
#include <string.h>
/* Return the number of records represented by nc use without changing their state. */
size_t umi_nc_use_count(const UmiNativeUseAnalysis *a,uint32_t id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL||id==0U)return 0U;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<a->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a->items[i].value_id==id)return a->items[i].uses;return 0U;}
/* Provide the nc use analyze operation used by this module and its client applications. */
UmiStatus umi_nc_use_analyze(const UmiNativeIrFunction *f,UmiNativeUseAnalysis *a){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||a==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(a,0,sizeof(*a));/* Visit each bounded item once so every record receives the same rule. */ for(size_t bi=0U;bi<f->block_count;bi++)/* Visit each bounded item once so every record receives the same rule. */ for(size_t ii=0U;ii<f->blocks[bi].instruction_count;ii++){const UmiNativeIrInstruction *in=&f->blocks[bi].instructions[ii];/* Visit each bounded item once so every record receives the same rule. */ for(size_t oi=0U;oi<in->operand_count;oi++){uint32_t id=in->operands[oi];size_t j=0U;/* Visit each bounded item once so every record receives the same rule. */ for(;j<a->count;j++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a->items[j].value_id==id){a->items[j].uses++;break;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(j==a->count){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a->count>=UMI_NC_MAX_ANALYZED_VALUES)return UMI_STATUS_CAPACITY_EXCEEDED;a->items[a->count].value_id=id;a->items[a->count].uses=1U;a->count++;}}}return UMI_STATUS_OK;}
