/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain/test_cvm_logical.c
 *
 * PURPOSE:
 *   Verify the shared preview adapter preserves logical short-circuit results, trace gaps and fresh failure state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/cvm_trace.h"
#include <stdio.h>
#include <string.h>
#ifndef UMC_CONDITIONAL_EXPRESSION_PROFILE
#error "Conditional preview qualification requires matching conditional-capable Umicom C headers and libraries."
#endif
#ifndef UMC_LOGICAL_CONNECTIVE_PROFILE
#error "Logical preview qualification requires matching logical-capable Umicom C headers and libraries."
#endif
#define CHECK(x) do {if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;}} while(0)
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *source="int main(void){int divisor=0;return divisor!=0 && 42/divisor>1;}";
    UmiCvmPreviewWorkspace workspace;UmiCvmPreviewResult result;UmiCvmPreviewTrace trace;
    CHECK(umi_cvm_preview_traced(source,strlen(source),8U,&workspace,sizeof workspace,
        &result,sizeof result,&trace,sizeof trace)==UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value==0 && result.instruction_count==16U &&
        result.execution.instructions_executed==8U && trace.count==8U);
    CHECK(trace.steps[5].opcode==UMC_JUMP_IF_ZERO && trace.steps[6].instruction_index==14U &&
        trace.steps[7].instruction_index==15U);
    /* The adapter must preserve a selected integer value, rather than turning
     * all successful branch-based expressions into Boolean presentation state. */
    if(strcmp(argv[1],"conditional-fallback")==0 || strcmp(argv[1],"conditional-true")==0){
        const bool choose_true=strcmp(argv[1],"conditional-true")==0;
        source=choose_true ? "int main(void){return -1 ? -9 : 1/0;}" : "int main(void){return 0 ? 1/0 : 42;}";
        CHECK(umi_cvm_preview_traced(source,strlen(source),UMC_PROGRAM_LIMIT,&workspace,sizeof workspace,
            &result,sizeof result,&trace,sizeof trace)==UMI_STATUS_OK);
        CHECK(result.completed && result.execution.value==(choose_true ? -9 : 42));
        CHECK(trace.count==result.execution.instructions_executed && trace.count<result.instruction_count);
        for(size_t i=0U;i<trace.count;++i)CHECK(trace.steps[i].opcode!=UMC_DIVIDE);
        return 0;
    }
    if(strcmp(argv[1],"conditional-fault")==0){
        source="int main(void){return 1 ? 1/0 : 42;}";
        CHECK(umi_cvm_preview_traced(source,strlen(source),UMC_PROGRAM_LIMIT,&workspace,sizeof workspace,
            &result,sizeof result,&trace,sizeof trace)==UMI_STATUS_INVALID_STATE);
        CHECK(!result.completed && result.execution.value==0 && result.execution.instructions_executed==0U);
        CHECK(result.phase==UMI_CVM_EXECUTING && trace.count>0U && trace.steps[trace.count-1U].opcode==UMC_DIVIDE);
        return 0;
    }
    if(strcmp(argv[1],"success")==0)return 0;
    if(strcmp(argv[1],"ordinary-parity")==0){
        UmiCvmPreviewResult ordinary;
        CHECK(umi_cvm_preview(source,strlen(source),8U,&workspace,sizeof workspace,&ordinary,sizeof ordinary)==UMI_STATUS_OK);
        CHECK(ordinary.completed && ordinary.execution.value==result.execution.value &&
            ordinary.execution.instructions_executed==8U && ordinary.instruction_count==16U);return 0;
    }
    if(strcmp(argv[1],"fuel-reset")==0){
        CHECK(umi_cvm_preview_traced(source,strlen(source),7U,&workspace,sizeof workspace,
            &result,sizeof result,&trace,sizeof trace)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!result.completed && result.execution.value==0 && result.execution.instructions_executed==0U &&
            result.phase==UMI_CVM_EXECUTING && trace.count==8U && trace.steps[7].instruction_index==15U &&
            trace.steps[7].status==UMC_FUEL_EXHAUSTED);return 0;
    }
    if(strcmp(argv[1],"syntax-reset")==0){
        source="int main(void){return 1 & 2;}";
        CHECK(umi_cvm_preview_traced(source,strlen(source),32U,&workspace,sizeof workspace,
            &result,sizeof result,&trace,sizeof trace)==UMI_STATUS_PARSE_ERROR);
        CHECK(!result.completed && result.execution.instructions_executed==0U && result.instruction_count==0U &&
            result.phase==UMI_CVM_COMPILING && trace.count==0U);return 0;
    }
    return 2;
}
