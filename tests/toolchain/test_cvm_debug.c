/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain/test_cvm_debug.c
 *
 * PURPOSE:
 *   Check paused lifecycle, fault coordinates and stale result invalidation through the shared compiler.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/cvm_debug.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} } while(0)
static UmiCvmDebugSession session;
static int finish(void)
{
    for(size_t i=0;i<UMC_PROGRAM_LIMIT && session.phase==UMI_CVM_DEBUG_PAUSED;++i)
        if(umi_cvm_debug_step(&session,sizeof session)!=UMC_OK)return 1;
    return session.phase!=UMI_CVM_DEBUG_COMPLETE;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);
    char source[]="int main(void){\nint x=40;\nx+=2;\nreturn x;\n}";
    CHECK(umi_cvm_debug_start(source,strlen(source),100,&session,sizeof session)==UMC_OK);
    CHECK(session.phase==UMI_CVM_DEBUG_PAUSED && session.machine.frame.executed==0);
    if(strcmp(argv[1],"lifetime")==0) {
        /* Stepping must not borrow the source or the compilation workspace. */
        memset(source,'?',sizeof source-1);
        memset(session.compilation,0,sizeof session.compilation);
        CHECK(!finish());CHECK(session.result.value==42);
        CHECK(umi_cvm_debug_step(&session,sizeof session)==UMC_INVALID_ARGUMENT);
    } else if(strcmp(argv[1],"cancel")==0) {
        CHECK(umi_cvm_debug_step(&session,sizeof session)==UMC_OK);
        size_t executed=session.machine.frame.executed;
        CHECK(umi_cvm_debug_cancel(&session,sizeof session)==UMC_OK);
        CHECK(session.phase==UMI_CVM_DEBUG_CANCELLED);
        CHECK(umi_cvm_debug_step(&session,sizeof session)==UMC_INVALID_ARGUMENT);
        CHECK(session.machine.frame.executed==executed && session.result.value==0);
    } else if(strcmp(argv[1],"restart")==0) {
        CHECK(!finish());CHECK(session.result.value==42);
        const char *bad="int main(void){return missing;}";
        CHECK(umi_cvm_debug_start(bad,strlen(bad),100,&session,sizeof session)!=UMC_OK);
        CHECK(session.phase==UMI_CVM_DEBUG_FAILED && session.result.value==0);
        CHECK(umi_cvm_debug_step(&session,sizeof session)==UMC_INVALID_ARGUMENT);
        CHECK(umi_cvm_debug_start(NULL,5,100,&session,sizeof session)==UMC_INVALID_ARGUMENT);
        CHECK(session.phase==UMI_CVM_DEBUG_FAILED && session.result.value==0);
    } else if(strcmp(argv[1],"fault")==0) {
        const char *bad="int main(void){\nreturn 1/0;\n}";
        CHECK(umi_cvm_debug_start(bad,strlen(bad),100,&session,sizeof session)==UMC_OK);
        UmcStatus status=UMC_OK;
        while(session.phase==UMI_CVM_DEBUG_PAUSED)status=umi_cvm_debug_step(&session,sizeof session);
        CHECK(status!=UMC_OK && session.phase==UMI_CVM_DEBUG_FAILED);
        CHECK(session.diagnostic.line==2 && session.diagnostic.column>0);
        CHECK(session.result.value==0);
    } else if(strcmp(argv[1],"fuel")==0) {
        CHECK(umi_cvm_debug_start(source,strlen(source),0,&session,sizeof session)==UMC_OK);
        CHECK(umi_cvm_debug_step(&session,sizeof session)!=UMC_OK);
        CHECK(session.phase==UMI_CVM_DEBUG_FAILED && session.machine.frame.executed==0);
    } else if(strcmp(argv[1],"size")==0) {
        UmiCvmDebugSession before=session;
        CHECK(umi_cvm_debug_start(source,strlen(source),100,&session,sizeof session-1)==UMC_INVALID_ARGUMENT);
        CHECK(memcmp(&before,&session,sizeof session)==0);
        CHECK(umi_cvm_debug_step(NULL,sizeof session)==UMC_INVALID_ARGUMENT);
        CHECK(umi_cvm_debug_cancel(&session,0)==UMC_INVALID_ARGUMENT);
        CHECK(memcmp(&before,&session,sizeof session)==0);
    } else return 2;
    return 0;
}
