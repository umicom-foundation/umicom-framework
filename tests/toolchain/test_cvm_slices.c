/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain/test_cvm_slices.c
 *
 * PURPOSE:
 *   Verify resume phase transitions, cancellation, argument preservation and source fault coordinates.
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
static UmiCvmDebugSession session, before;
int main(void)
{
    const char *source="int main(void){\nint x=40;\nx+=2;\nreturn x;\n}";
    for(size_t budget=1;budget<=UMC_PROGRAM_LIMIT;++budget) {
        CHECK(umi_cvm_debug_start(source,strlen(source),100,&session,sizeof session)==UMC_OK);
        while(session.phase==UMI_CVM_DEBUG_PAUSED) {
            size_t previous=session.machine.frame.executed;
            CHECK(umi_cvm_debug_resume(&session,sizeof session,budget)==UMC_OK);
            CHECK(session.machine.frame.executed>previous && session.machine.frame.executed-previous<=budget);
        }
        CHECK(session.phase==UMI_CVM_DEBUG_COMPLETE && session.result.value==42);
        before=session;
        CHECK(umi_cvm_debug_resume(&session,sizeof session,2)==UMC_INVALID_ARGUMENT);
        CHECK(memcmp(&before,&session,sizeof session)==0);
    }
    CHECK(umi_cvm_debug_start(source,strlen(source),100,&session,sizeof session)==UMC_OK);
    before=session;
    CHECK(umi_cvm_debug_resume(&session,sizeof session,0)==UMC_INVALID_ARGUMENT);
    CHECK(umi_cvm_debug_resume(&session,sizeof session,SIZE_MAX)==UMC_INVALID_ARGUMENT);
    CHECK(umi_cvm_debug_resume(&session,sizeof session-1,1)==UMC_INVALID_ARGUMENT);
    CHECK(umi_cvm_debug_resume(NULL,sizeof session,1)==UMC_INVALID_ARGUMENT);
    CHECK(memcmp(&before,&session,sizeof session)==0);
    CHECK(umi_cvm_debug_resume(&session,sizeof session,2)==UMC_OK);
    CHECK(umi_cvm_debug_cancel(&session,sizeof session)==UMC_OK);
    before=session;
    CHECK(umi_cvm_debug_resume(&session,sizeof session,2)==UMC_INVALID_ARGUMENT);
    CHECK(memcmp(&before,&session,sizeof session)==0);
    const char *bad="int main(void){\nreturn 1/0;\n}";
    CHECK(umi_cvm_debug_start(bad,strlen(bad),100,&session,sizeof session)==UMC_OK);
    CHECK(umi_cvm_debug_resume(&session,sizeof session,10)==UMC_ARITHMETIC_ERROR);
    CHECK(session.phase==UMI_CVM_DEBUG_FAILED && session.diagnostic.line==2 && session.diagnostic.column==9);
    CHECK(session.machine.frame.executed==2 && session.result.value==0 && session.last_step.instruction_index==2);
    CHECK(umi_cvm_debug_start(source,strlen(source),1,&session,sizeof session)==UMC_OK);
    CHECK(umi_cvm_debug_resume(&session,sizeof session,10)==UMC_FUEL_EXHAUSTED);
    CHECK(session.phase==UMI_CVM_DEBUG_FAILED && session.machine.frame.executed==1 && session.result.value==0);
    return 0;
}
