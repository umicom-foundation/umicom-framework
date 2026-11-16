/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/cvm_debug.h
 *
 * PURPOSE:
 *   Own application-facing debugger lifecycle while delegating execution to the Umicom C SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TOOLCHAIN_CVM_DEBUG_H
#define UMICOM_TOOLCHAIN_CVM_DEBUG_H
#include "umicom_c/machine.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiCvmDebugPhase { UMI_CVM_DEBUG_EMPTY,UMI_CVM_DEBUG_PAUSED,UMI_CVM_DEBUG_COMPLETE,UMI_CVM_DEBUG_FAILED,UMI_CVM_DEBUG_CANCELLED } UmiCvmDebugPhase;
typedef struct UmiCvmDebugSession {
    UmcMachine machine;
    UmcInstruction compilation[UMC_PROGRAM_LIMIT];
    size_t lines[UMC_PROGRAM_LIMIT],columns[UMC_PROGRAM_LIMIT];
    UmcDiagnostic diagnostic;
    UmcTraceStep last_step;
    UmcResult result;
    UmiCvmDebugPhase phase;
} UmiCvmDebugSession;
/* Caller owns one session per concurrent preview. Fields may be inspected but
 * not mutated. Start before step/cancel. Accepted storage invalidates the prior
 * session before source admission; failure cannot expose an old ready result.
 * Source and session storage must be valid and disjoint.
 * Source is borrowed for start only. Location tables preserve original byte
 * coordinates after source storage is released. No native code, heap, host
 * calls, GUI callbacks or network effects are enabled by this adapter.
 * Native UmcStatus is retained to distinguish syntax, fuel and arithmetic. */
UmcStatus umi_cvm_debug_start(const char *source,size_t length,size_t fuel,UmiCvmDebugSession *session,size_t size);
UmcStatus umi_cvm_debug_step(UmiCvmDebugSession *session,size_t size);
UmcStatus umi_cvm_debug_cancel(UmiCvmDebugSession *session,size_t size);
#ifdef __cplusplus
}
#endif
#endif
