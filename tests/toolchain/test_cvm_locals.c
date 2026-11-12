/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain/test_cvm_locals.c
 *
 * PURPOSE:
 *   Check shared preview state and status translation for initialized local integer programs.
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
#ifndef UMC_LOCAL_INTEGER_PROFILE
#error "Local preview qualification requires matching local-capable Umicom C headers and libraries."
#endif
#define CHECK(x) do { if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1];
    const char *source="int main(void){int x=6; x=x*7; return x;}";
    UmiCvmPreviewWorkspace workspace; UmiCvmPreviewResult result; UmiCvmPreviewTrace trace;
    CHECK(umi_cvm_preview_traced(source,strlen(source),8U,&workspace,sizeof workspace,
        &result,sizeof result,&trace,sizeof trace)==UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value==42 && trace.count==8U);
    CHECK(trace.steps[1].opcode==UMC_STORE_LOCAL && trace.steps[1].stack_depth==0U);
    CHECK(trace.steps[2].opcode==UMC_LOAD_LOCAL && trace.steps[2].top==6);
    size_t fuel=8U; UmiStatus expected=UMI_STATUS_OK; char many[2048];
    if(strcmp(mode,"syntax-reset")==0){source="int main(void){int x=x;return x;}";expected=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"fuel-reset")==0){fuel=5U;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(mode,"capacity-reset")==0){
        size_t used=(size_t)snprintf(many,sizeof many,"int main(void){");
        for(size_t i=0U;i<33U;++i){
            int n=snprintf(many+used,sizeof many-used,"int v%zu=0;",i);
            CHECK(n>0 && (size_t)n<sizeof many-used);used+=(size_t)n;
        }
        CHECK(snprintf(many+used,sizeof many-used,"return 0;}")>0);
        source=many;expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(strcmp(mode,"ordinary-parity")==0){
        UmiCvmPreviewResult ordinary;
        CHECK(umi_cvm_preview(source,strlen(source),8U,&workspace,sizeof workspace,
            &ordinary,sizeof ordinary)==UMI_STATUS_OK);
        CHECK(ordinary.completed && ordinary.execution.value==42 && ordinary.instruction_count==8U);
    }else if(strcmp(mode,"success")!=0)return 2;
    CHECK(umi_cvm_preview_traced(source,strlen(source),fuel,&workspace,sizeof workspace,
        &result,sizeof result,&trace,sizeof trace)==expected);
    if(expected!=UMI_STATUS_OK){
        CHECK(!result.completed && result.execution.value==0 && result.execution.instructions_executed==0U);
        if(strcmp(mode,"fuel-reset")==0){
            CHECK(result.phase==UMI_CVM_EXECUTING && trace.count==6U);
            CHECK(trace.steps[5].opcode==UMC_STORE_LOCAL && trace.steps[5].status==UMC_FUEL_EXHAUSTED);
        }else{
            CHECK(result.phase==UMI_CVM_COMPILING && trace.count==0U && result.instruction_count==0U);
            CHECK(result.diagnostic.status==(expected==UMI_STATUS_PARSE_ERROR?UMC_SYNTAX_ERROR:UMC_LOCAL_CAPACITY));
        }
    }
    return 0;
}
