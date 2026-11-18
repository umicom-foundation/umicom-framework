/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain/test_cvm_shifts.c
 *
 * PURPOSE:
 *   Verify canonical checked shifts retain fresh preview state across success and refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/cvm_trace.h"
#include "umicom_c/profile.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1; } } while(0)

int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    CHECK(umc_compiler_profile(UMC_PROFILE_FEATURES)&UMC_FEATURE_CHECKED_SHIFTS);
    CHECK(umc_runtime_profile(UMC_PROFILE_FEATURES)&UMC_FEATURE_CHECKED_SHIFTS);
    const char *source="int main(void){return 21<<1;}";
    UmiCvmPreviewWorkspace workspace;
    UmiCvmPreviewResult result;
    UmiCvmPreviewTrace trace;
    CHECK(umi_cvm_preview_traced(source,strlen(source),4U,&workspace,sizeof workspace,
        &result,sizeof result,&trace,sizeof trace)==UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value==42 && trace.count==4U);
    CHECK(trace.steps[2].opcode==UMC_SHIFT_LEFT && trace.steps[2].top==42);
    if(strcmp(argv[1],"left")==0)return 0;
    if(strcmp(argv[1],"right")==0 || strcmp(argv[1],"updates")==0 ||
        strcmp(argv[1],"short-circuit")==0) {
        source=strcmp(argv[1],"right")==0 ? "int main(void){return 85>>1;}" :
            strcmp(argv[1],"updates")==0 ? "int main(void){int x=84;x>>=1;x<<=0;return x;}" :
            "int main(void){return 0&&(1<<32)?0:42;}";
        CHECK(umi_cvm_preview_traced(source,strlen(source),UMC_PROGRAM_LIMIT,&workspace,sizeof workspace,
            &result,sizeof result,&trace,sizeof trace)==UMI_STATUS_OK);
        CHECK(result.completed && result.execution.value==42);
        UmiCvmPreviewResult ordinary;
        CHECK(umi_cvm_preview(source,strlen(source),UMC_PROGRAM_LIMIT,&workspace,sizeof workspace,
            &ordinary,sizeof ordinary)==UMI_STATUS_OK);
        CHECK(ordinary.completed && ordinary.execution.value==42 &&
            ordinary.execution.instructions_executed==result.execution.instructions_executed);
        if(strcmp(argv[1],"short-circuit")==0)
            for(size_t i=0U;i<trace.count;++i)CHECK(trace.steps[i].opcode!=UMC_SHIFT_LEFT);
        return 0;
    }
    if(strcmp(argv[1],"fault-reset")==0) {
        const char *faults[]={"int main(void){return 1<<31;}","int main(void){return 1>>32;}",
            "int main(void){return -1>>0;}","int main(void){return 1<<-1;}"};
        for(size_t i=0U;i<sizeof faults/sizeof faults[0];++i) {
            CHECK(umi_cvm_preview_traced(faults[i],strlen(faults[i]),UMC_PROGRAM_LIMIT,
                &workspace,sizeof workspace,&result,sizeof result,&trace,sizeof trace)==UMI_STATUS_INVALID_STATE);
            CHECK(!result.completed && result.execution.value==0 &&
                result.execution.instructions_executed==0U && result.phase==UMI_CVM_EXECUTING);
            CHECK(trace.count>0U && trace.steps[trace.count-1U].status==UMC_ARITHMETIC_ERROR);
            CHECK(trace.steps[trace.count-1U].stack_depth==2U);
            CHECK(umi_cvm_preview_traced(source,strlen(source),4U,&workspace,sizeof workspace,
                &result,sizeof result,&trace,sizeof trace)==UMI_STATUS_OK);
            CHECK(result.completed && result.execution.value==42);
        }
        return 0;
    }
    if(strcmp(argv[1],"syntax-reset")==0) {
        source="int main(void){return 1<<<1;}";
        CHECK(umi_cvm_preview_traced(source,strlen(source),UMC_PROGRAM_LIMIT,&workspace,sizeof workspace,
            &result,sizeof result,&trace,sizeof trace)==UMI_STATUS_PARSE_ERROR);
        CHECK(!result.completed && result.execution.value==0 && result.instruction_count==0U &&
            result.phase==UMI_CVM_COMPILING && trace.count==0U);
        return 0;
    }
    if(strcmp(argv[1],"fuel-reset")==0) {
        CHECK(umi_cvm_preview_traced(source,strlen(source),2U,&workspace,sizeof workspace,
            &result,sizeof result,&trace,sizeof trace)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!result.completed && result.execution.value==0 && result.phase==UMI_CVM_EXECUTING);
        CHECK(trace.count==3U && trace.steps[2].opcode==UMC_SHIFT_LEFT &&
            trace.steps[2].status==UMC_FUEL_EXHAUSTED && trace.steps[2].stack_depth==2U);
        return 0;
    }
    return 2;
}
