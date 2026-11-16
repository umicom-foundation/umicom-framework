/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/cvm_debug.c
 *
 * PURPOSE:
 *   Invalidate stale UI results and retain source coordinates across paused native bytecode execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/cvm_debug.h"
#include <string.h>
UmcStatus umi_cvm_debug_start(const char *source,size_t length,size_t fuel,UmiCvmDebugSession *session,size_t size)
{
    if(!session || size!=sizeof *session)return UMC_INVALID_ARGUMENT;
    /* Presentation state belongs to this request, never a prior successful run. */
    memset(session,0,sizeof *session);
    session->phase=UMI_CVM_DEBUG_FAILED;
    size_t count=0;
    UmcStatus status=umc_compile(source,length,session->compilation,UMC_PROGRAM_LIMIT,&count,&session->diagnostic);
    if(status!=UMC_OK)return status;
    for(size_t i=0;i<count;++i) {
        UmcDiagnostic location={.source_offset=session->compilation[i].source_offset};
        umc_locate_diagnostic(source,length,&location);
        session->lines[i]=location.line;session->columns[i]=location.column;
    }
    status=umc_machine_init(&session->machine,session->compilation,count,fuel,&session->diagnostic);
    if(status==UMC_OK)session->phase=UMI_CVM_DEBUG_PAUSED;
    return status;
}
UmcStatus umi_cvm_debug_step(UmiCvmDebugSession *session,size_t size)
{
    if(!session || size!=sizeof *session || session->phase!=UMI_CVM_DEBUG_PAUSED)return UMC_INVALID_ARGUMENT;
    const size_t index=session->machine.frame.pc;
    UmcStatus status=umc_machine_step(&session->machine,&session->last_step,&session->diagnostic);
    if(status!=UMC_OK) {
        session->phase=UMI_CVM_DEBUG_FAILED;
        if(index<UMC_PROGRAM_LIMIT){session->diagnostic.line=session->lines[index];session->diagnostic.column=session->columns[index];}
        return status;
    }
    if(session->machine.frame.finished) {
        status=umc_machine_result(&session->machine,&session->result,&session->diagnostic);
        session->phase=status==UMC_OK?UMI_CVM_DEBUG_COMPLETE:UMI_CVM_DEBUG_FAILED;
    }
    return status;
}
UmcStatus umi_cvm_debug_cancel(UmiCvmDebugSession *session,size_t size)
{
    if(!session || size!=sizeof *session || session->phase!=UMI_CVM_DEBUG_PAUSED)return UMC_INVALID_ARGUMENT;
    session->phase=UMI_CVM_DEBUG_CANCELLED;
    return UMC_OK;
}
