/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/cvm_preview.h
 *
 * PURPOSE:
 *   Expose bounded source preview with explicit result ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_CVM_PREVIEW_H
#define UMICOM_TOOLCHAIN_CVM_PREVIEW_H
#include "umicom/base/status.h"
#include "umicom_c/umicom_c.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiCvmPreviewPhase {
    UMI_CVM_NOT_STARTED, UMI_CVM_COMPILING, UMI_CVM_EXECUTING, UMI_CVM_COMPLETE
} UmiCvmPreviewPhase;
typedef struct UmiCvmPreviewWorkspace {
    UmcInstruction instructions[UMC_PROGRAM_LIMIT];
} UmiCvmPreviewWorkspace;
typedef struct UmiCvmPreviewResult {
    UmiCvmPreviewPhase phase;
    bool completed;
    size_t instruction_count;
    UmcResult execution;
    UmcDiagnostic diagnostic;
} UmiCvmPreviewResult;
/* Workspace and result are stable caller-owned, disjoint objects. A call stores
 * no pointers for later use. Use one workspace per concurrent preview.
 * Wrong storage sizes fail without touching either object. Once storage is
 * accepted, a new request invalidates the previous result before compilation. */
UmiStatus umi_cvm_preview(const char *source, size_t length, size_t fuel,
    UmiCvmPreviewWorkspace *workspace, size_t workspace_size,
    UmiCvmPreviewResult *result, size_t result_size);
#ifdef __cplusplus
}
#endif
#endif
