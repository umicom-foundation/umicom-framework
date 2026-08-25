/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/dead_code_elimination.c
 *
 * PURPOSE:
 *   Remove unused side-effect-free IR instructions while retaining terminators and observable operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/dead_code_elimination.h"
#include "umicom/compiler/native/use_analysis.h"
UmiStatus umi_nc_dead_code_eliminate(UmiNativeIrFunction *f,size_t *out){if(f==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;UmiNativeUseAnalysis use;UmiStatus st=umi_nc_use_analyze(f,&use);if(st!=UMI_STATUS_OK)return st;size_t removed=0U;for(size_t bi=0U;bi<f->block_count;bi++){UmiNativeIrBlock *b=&f->blocks[bi];size_t w=0U;for(size_t i=0U;i<b->instruction_count;i++){UmiNativeIrInstruction *in=&b->instructions[i];bool drop=umi_nc_ir_instruction_produces_value(in)&&!in->side_effect&&umi_nc_use_count(&use,in->result_id)==0U;if(drop){removed++;continue;}if(w!=i)b->instructions[w]=*in;w++;}b->instruction_count=w;}*out=removed;return UMI_STATUS_OK;}
